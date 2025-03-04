#include "ScrollManager.h"
#include "Utils.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void ScrollManager::ReplaceBookWithScroll(RE::TESObjectBOOK* book) {

    if (!book) {
        return;
    }

    if (!book->IsBookTome()) {
        return;
    }

    auto id = book->GetFormID();
    auto editor = Utils::GetEditorId(book);

    if (auto scroll = Utils::CreateFormByType<RE::ScrollItem>()) {
        scrollData[scroll] = new ScrollData(nullptr, scroll, book);
        scroll->SetFormID(id, false);

        logger::info("Replacing book with scroll: {:x} {}", id, editor);

        scroll->SetFormEditorID(editor.c_str());

        {
            const auto& [map, lock] = RE::TESForm::GetAllFormsByEditorID();
            const RE::BSReadWriteLock l{lock};

            map->erase(editor);
            map->insert(RE::BSTTuple(editor, scroll));
        }

        {
            const auto& [map, lock] = RE::TESForm::GetAllForms();
            const RE::BSReadWriteLock l{lock};

            map->erase(id);
            map->insert(RE::BSTTuple(id, scroll));
        }

    }
}

void ScrollManager::ApplyEffectsToScroll(ScrollData* data) {
    auto book = data->OriginalItem;

    auto spell = book->GetSpell();

    if (!spell) {
        return;
    }
    data->BaseSpell = spell;

    auto df = data->Scroll;

    auto model = book->GetModel();
    auto name = book->GetName();
    auto value = book->GetGoldValue();
    auto weight = book->GetWeight();

    if (auto newBook = df->As<RE::ScrollItem>()) {
        auto effectLevel = new RE::Effect();
        effectLevel->baseEffect = levelEffect;
        effectLevel->cost = 0;

        auto effectCost = new RE::Effect();
        effectCost->cost = 0;
        effectCost->baseEffect = spell->GetCastingType() == RE::MagicSystem::CastingType::kConcentration
                                        ? costPerSecoundEffect
                                        : costEffect;

        newBook->effects.push_back(effectLevel);
        newBook->effects.push_back(effectCost);

        newBook->CopyMagicItemData(spell);

        for (auto effect : spell->effects) {
            auto copy = new RE::Effect();
            copy->effectItem = effect->effectItem;

            copy->baseEffect = effect->baseEffect;
            copy->cost = effect->cost;
            copy->conditions = effect->conditions;

            newBook->effects.push_back(copy);
        }

        for (auto key : book->GetKeywords()) {
            newBook->AddKeyword(key);
        }

        newBook->AddKeyword(keyword);
        auto it = replaceModels.find(model);
        if (it != replaceModels.end()) {
            newBook->SetModel(it->second.c_str());
        } else {
            newBook->SetModel(model);
        }
        newBook->weight = weight;
        newBook->value = value;
        newBook->SetFullName(name);

        newBook->SpellItem::data = spell->data;
        newBook->SpellItem::hostileCount = spell->hostileCount;
        newBook->SpellItem::avEffectSetting = spell->avEffectSetting;
        newBook->equipSlot = spell->equipSlot;
        scrollData[newBook] = new ScrollData(spell, newBook);
        ApplyLevel(newBook);

        effectLevel->effectItem.magnitude = 1;

        effectCost->effectItem.magnitude = newBook->CalculateMagickaCost(RE::PlayerCharacter::GetSingleton());
    }
}

ScrollData* ScrollManager::GetScrollData(RE::SpellItem* item) { 
    auto it = scrollData.find(item);
    if (it != scrollData.end()) {
        return it->second;
    }
    return nullptr;
}

PlayerLevel* ScrollManager::GetPlayerSkill(RE::SpellItem* item) {
    auto it = playerSkill.find(item);
    if (it != playerSkill.end()) {
        return it->second;
    }
    return nullptr;
}

ScrollLevel* ScrollManager::GetScrollLevel(RE::SpellItem* scroll) { 
    auto data = GetScrollData(scroll);

    if (data && RE::PlayerCharacter::GetSingleton()->HasSpell(data->BaseSpell)) {
        return scrollLevels.back();
    }

    ScrollLevel* last = nullptr;
    auto level = GetPlayerSkill(scroll);
    if (!level) {
        return scrollLevels.front();
    }
    for (auto scrollLevel : scrollLevels) {
        if (scrollLevel->casts >= level->casts) {
            return last;
        }
        last = scrollLevel;
    }
    return last;
}

void ScrollManager::ApplyLevel(RE::SpellItem* scroll) {
    auto data = GetScrollData(scroll);
    if (data) {
        auto level = GetScrollLevel(scroll);
        if (level) {
            auto base = data->BaseSpell;
            auto scroll = data->Scroll;
            scroll->data.costOverride = base->data.costOverride * level->costPercentage / 100;
            for (int i = scroll->effects.size()-1; i >= 0; i--) {
                if (i < 2) {
                    if (scroll->effects[i]->baseEffect == costPerSecoundEffect ||
                        scroll->effects[i]->baseEffect == costEffect) {
                        scroll->effects[i]->effectItem.magnitude =
                            scroll->CalculateMagickaCost(RE::PlayerCharacter::GetSingleton());
                    } else if (scroll->effects[i]->baseEffect == levelEffect) {
                        scroll->effects[i]->effectItem.magnitude = level->level;
                    }
                } else {
                    if (i - 2 < base->effects.size()) {
                        scroll->effects[i]->effectItem.magnitude = base->effects[i - 2]->effectItem.magnitude * level->magnitudePercentage / 100;
                        scroll->effects[i]->effectItem.duration = base->effects[i - 2]->effectItem.duration * level->durationPercentage / 100;
                        scroll->effects[i]->cost = base->effects[i - 2]->cost * level->costPercentage / 100;
                    } 
                }
            }
        } else {
            auto base = data->BaseSpell;
            auto scroll = data->Scroll;
            scroll->data.costOverride = base->data.costOverride;
            for (int i = scroll->effects.size() - 1; i >= 0; i--) {
                if (i < 2) {
                    if (scroll->effects[i]->baseEffect == costPerSecoundEffect ||
                        scroll->effects[i]->baseEffect == costEffect) {
                        scroll->effects[i]->effectItem.magnitude = scroll->CalculateMagickaCost(RE::PlayerCharacter::GetSingleton());
                    } 
                    else 
                        if (scroll->effects[i]->baseEffect == levelEffect) 
                    {
                        scroll->effects[i]->effectItem.magnitude = maxLevel;
                    }
                } else {
                    if (i - 2 < base->effects.size()) {
                        scroll->effects[i]->effectItem.magnitude = base->effects[i - 2]->effectItem.magnitude;
                        scroll->effects[i]->effectItem.duration = base->effects[i - 2]->effectItem.duration;
                        scroll->effects[i]->cost = base->effects[i - 2]->cost;
                    } 
                }
            }
        }
    }
}

void ScrollManager::HandleLevelUp(RE::SpellItem* spell) {
    auto now = RE::Calendar::GetSingleton()->GetHoursPassed();

    if (playerSkill.find(spell) == playerSkill.end()) {
        playerSkill[spell] = new PlayerLevel(0);
    }

    if (spell->data.castingType != RE::MagicSystem::CastingType::kConcentration || playerSkill[spell]->CanLevelUp()) {
        playerSkill[spell]->casts++;
        playerSkill[spell]->lastLevelUp = now;
        ApplyLevel(spell);
        auto data = GetScrollData(spell);
        auto level = GetScrollLevel(spell);
        auto skill = GetPlayerSkill(spell);
        if (data) {
                if (level && skill) {
                    if (skill->lastLevelCasts != level->casts) 
                    {
                        skill->lastLevelCasts = level->casts;
                        RE::DebugNotification(std::format("Your Knowledge of The Spell {} is at Level {}", data->BaseSpell->GetName(), level->level).c_str());
                    }
                } 
                else 
                {
                    auto player = RE::PlayerCharacter::GetSingleton();
                    if (!player->HasSpell(data->BaseSpell)) {
                        player->AddSpell(data->BaseSpell);
                        RE::DebugNotification(std::format("You Mastered the Spell {}", data->BaseSpell->GetName()).c_str());
                    }
                }
        }
    }
}

playerSkillMap& ScrollManager::GetTimesCastMap() {
    return playerSkill; }

float ScrollManager::GetLevelUpCooldown() { return concentrationCastingTimeCooldown; }

void ScrollManager::SaveGame(Serializer* serializer) {
    serializer->Write<uint32_t>(playerSkill.size());
    for (auto [key, value] : playerSkill) {
        serializer->WriteForm(key);
        serializer->Write<uint32_t>(value->casts);
        serializer->Write<uint32_t>(value->lastLevelCasts);
    }
}

void ScrollManager::LoadGame(Serializer* serializer) {
    auto length = serializer->Read<uint32_t>();

    for (uint32_t i = 0; i < length; i++) 
    {
        auto form = serializer->ReadForm<RE::SpellItem>();
        auto level = serializer->Read<uint32_t>();
        auto lastLevelCasts = serializer->Read<uint32_t>();
        playerSkill[form] = new PlayerLevel(level, lastLevelCasts);
        ApplyLevel(form);
    }
}

void ScrollManager::CleanLevel() {
    for (auto [key, value] : playerSkill) {
        value->casts = 0;
        ApplyLevel(key);
        delete value;
    }

    playerSkill.clear();
}

void ScrollManager::ReadConfigFile() {
    const std::string filename = "Data\\SKSE\\Plugins\\BooksOfPower.json";

    std::ifstream file(filename);

    if (!file) {
        logger::error("BooksOfPower.json not found");
        return;
    }

    try {
        const json data = json::parse(file);

        logger::info("Reading: DisplayBookOnTheOtherHand");
        if (data.contains("DisplayBookOnTheOtherHand")) {
            if (data["DisplayBookOnTheOtherHand"].is_boolean()) {
                displayBookOnTheOtherHand = data["DisplayBookOnTheOtherHand"].get<float>();
                logger::info("Set DisplayBookOnTheOtherHand as {}", displayBookOnTheOtherHand);
            } else {
                logger::error("DisplayBookOnTheOtherHand must be a boolean");
            }
        } else {
            logger::error("Missing DisplayBookOnTheOtherHand");
        }


        logger::info("Reading: ConcentrationCastTimeCooldown");
        if (data.contains("ConcentrationCastTimeCooldown")) {
            if (data["ConcentrationCastTimeCooldown"].is_number()) {
                concentrationCastingTimeCooldown = data["ConcentrationCastTimeCooldown"].get<float>();
                logger::info("Set cast time cooldown as {}", concentrationCastingTimeCooldown);
            } else {
                logger::error("ConcentrationCastTimeCooldown must be a number");
            }
        }
        else {
            logger::error("Missing CastTimeCooldown");
        }
        logger::info("Reading: HandBookDisplayItems");

        if (data.contains("HandBookDisplayItems")) {
            if (data["HandBookDisplayItems"].is_array()) {
                for (auto item : data["HandBookDisplayItems"]) {
                    if (item.contains("ActorValue") && item.contains("BookEditorId")) {
                        if (item["ActorValue"].is_string() && item["BookEditorId"].is_string()) {
                            auto avString = item["ActorValue"].get<std::string>();
                            auto editorId = item["BookEditorId"].get<std::string>();
                            auto av = Utils::ActorValueFromString(avString);
                            handBooks[av] = new HandBook(editorId);
                            logger::info("Registered HandBookDisplayItems BookEditorId: {} ActorValue: {}", editorId, av);
                        } else {
                            logger::error("ActorValue and BookEditorId must be string");
                        }
                    } else {
                        logger::error("Missing ActorValue and or BookEditorId");
                    }
                }
            } else {
                logger::error("HandBookDisplayItems must be an array");
            }
        } else {
            logger::error("Missing HandBookDisplayItems");
        }

        logger::info("Reading: BookModelSwap");
        if (data.contains("BookModelSwap")) {
            if (data["BookModelSwap"].is_array()) {
                for (auto item : data["BookModelSwap"]) {
                    if (item.contains("From") && item.contains("To")) {
                        if (item["From"].is_string() && item["To"].is_string()) {
                            auto from = item["From"].get<std::string>();
                            auto to = item["To"].get<std::string>();
                            replaceModels[from] = to;
                            logger::info("Registered BookModelSwap From: {} To: {}", from, to);

                        } else {
                            logger::error("From and To must be string");
                        }
                    } else {
                        logger::error("Missing From and or To");
                    }
                }
            } else {
                logger::error("BookModelSwap must be an array");
            }
        } else {
            logger::error("Missing BookModelSwap");
        }

        logger::info("Reading: ScrollLevels");
        if (data.contains("ScrollLevels")) {
            if (data["ScrollLevels"].is_array()) {
                uint32_t level = 1;
                uint32_t lastCasts = 0;
                for (auto item : data["ScrollLevels"]) {

                    uint32_t casts = 0;
                    float magnitudePercentage = 0;
                    float durationPercentage = 0;
                    float costPercentage = 0;

                    if (item.contains("NumberOfSuccessfulCasts")) {
                        if (item["NumberOfSuccessfulCasts"].is_number_integer()) {
                            casts = item["NumberOfSuccessfulCasts"].get<uint32_t>();
                        } else {
                            logger::error("NumberOfSuccessfulCasts must be integer");
                        }
                    } 

                    if (item.contains("MagnitudePercentage")) {
                        if (item["MagnitudePercentage"].is_number()) {
                            magnitudePercentage = item["MagnitudePercentage"].get<float>();
                        } else {
                            logger::error("MagnitudePercentage must be a number");
                        }
                    }

                    if (item.contains("DurationPercentage")) {
                        if (item["DurationPercentage"].is_number()) {
                            durationPercentage = item["DurationPercentage"].get<float>();
                        } else {
                            logger::error("DurationPercentage must be a number");
                        }
                    }

                    if (item.contains("CostPercentage")) {
                        if (item["CostPercentage"].is_number()) {
                            costPercentage = item["CostPercentage"].get<float>();
                        } else {
                            logger::error("CostPercentage must be a number");
                        }
                    }

                    if (casts >= lastCasts) {
                        scrollLevels.push_back(new ScrollLevel(casts, magnitudePercentage, durationPercentage, costPercentage, level));
                        logger::info("Registered ScrollLevels NumberOfSuccessfulCasts: {}, MagnitudePercentage: {}, DurationPercentage: {}, CostPercentage: {}, ", casts, magnitudePercentage, durationPercentage, costPercentage);
                        level++;
                        lastCasts = casts;
                    } else {
                        logger::error("NumberOfSuccessfulCasts must be ascending ordered last: {} current: {}", lastCasts, casts);
                    }
                }
                maxLevel = level-1;
            } else {
                logger::error("ScrollLevels must be an array");
            }
        } else {
            logger::error("Missing ScrollLevels");
        }


    } catch (const json::parse_error&) {  // NOLINT(bugprone-empty-catch)
    }
}

void ScrollManager::DataLoaded() {

    ReadConfigFile();

    keyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("BOP_ChannelingTome");
    costEffect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("BOP_MagickaCostDisplay");
    costPerSecoundEffect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("BOP_MagickaCostDisplayPerSecond");
    levelEffect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("BOP_LevelDisplay");

    for (auto [key, value] : scrollData) {
        ApplyEffectsToScroll(value);
    }


    //const auto& [map, lock] = RE::TESForm::GetAllForms();
    //const RE::BSReadWriteLock l{lock};
    //for (auto& [id, form] : *map) {
    //    if (form) {
    //        if (auto book = form->As<RE::TESObjectBOOK>()) {
    //                ReplaceSpellTome(book);
    //        }
    //    }
    //}
}

bool ScrollManager::OnEquip(RE::Actor* player, RE::TESBoundObject* a_object) {
    if (!displayBookOnTheOtherHand) {
        return true;
    }

    if (!a_object) {
        return true;
    }

    if (!player || !player->IsPlayerRef()) {
        return true;
    }

    if (DefaultBehavior) {
        return true;
    }

    auto leftHandItem = player->GetEquippedObject(true);
    auto rightHandItem = player->GetEquippedObject(false);
    auto left = Utils::GetSlot(true);
    auto right = Utils::GetSlot(false);

    if (leftHandItem && leftHandItem->HasKeywordByEditorID("BOP_HandBook")) {

        if (rightHandItem) {
            if (auto bound = rightHandItem->As<RE::TESBoundObject>()) {
                DefaultBehavior = true;
                RE::ActorEquipManager::GetSingleton()->UnequipObject(player, bound, nullptr, 1, right, false, true,
                                                                     false);
                DefaultBehavior = false;
            }
        }

    }

    if (rightHandItem && rightHandItem->HasKeywordByEditorID("BOP_ChannelingTome")) {
        if (leftHandItem) {
            if (auto bound = leftHandItem->As<RE::TESBoundObject>()) {
                DefaultBehavior = true;
                RE::ActorEquipManager::GetSingleton()->UnequipObject(player, bound, nullptr, 1, left, false, true,false);
                DefaultBehavior = false;
            }
        }
    }

    if (a_object->HasKeywordByEditorID("BOP_ChannelingTome")) {
        if (auto scroll = a_object->As<RE::ScrollItem>()) {

            auto data = GetScrollData(scroll);
            if (data) {
                auto it = handBooks.find(data->BaseSpell->GetAssociatedSkill());
                if (it != handBooks.end()) {
                    it->second->Equip(player);
                    DefaultBehavior = true;
                    RE::ActorEquipManager::GetSingleton()->EquipObject(player, a_object, nullptr, 1, right, false, false, false,
                                                                       true);

                    DefaultBehavior = false;
                    return false;
                }
            }
        }

    }
    return true;
}

void ScrollManager::OnUnEquip(RE::Actor* player, RE::TESBoundObject* a_object) {

    if (!displayBookOnTheOtherHand) {
        return;
    }

    if (!a_object) {
        return;
    }

    if (!player || !player->IsPlayerRef()) {
        return;
    }
    if (DefaultBehavior) {
        return;
    }

    if (a_object->HasKeywordByEditorID("BOP_ChannelingTome")) {
        auto player = RE::PlayerCharacter::GetSingleton();
        auto slot = Utils::GetSlot(true);
        auto obj = player->GetEquippedObject(true);
        if (obj) {
            if (auto bound = obj->As<RE::TESObjectWEAP>()) {
                if (bound->HasKeywordByEditorID("BOP_HandBook")) {
                    RE::ActorEquipManager::GetSingleton()->UnequipObject(player, bound, nullptr, 1, slot, false, true, false);
                }
            }
        }
    }
}

void ScrollManager::OnCast(RE::Actor* caster, RE::SpellItem* spell) {
    if (spell->HasKeywordByEditorID("BOP_ChannelingTome")) {
        HandleLevelUp(spell);
    }
}


RE::TESObjectWEAP* HandBook::GetWeapon() {
    if (weapon == nullptr) {
        weapon = RE::TESForm::LookupByEditorID<RE::TESObjectWEAP>(weaponEditorId);
    }
    return weapon;
}

void HandBook::Equip(RE::Actor* actor) {
    auto obj = GetWeapon();

    if (obj == nullptr) {
        return;
    }

    if (actor == nullptr) {
        return;
    }
    auto inv = actor->GetInventory();

    if (inv.find(obj) == inv.end()) {
        actor->AddObjectToContainer(obj, nullptr, 1, nullptr);
    }
    auto left = Utils::GetSlot(true);

    RE::ActorEquipManager::GetSingleton()->EquipObject(actor, obj, nullptr, 1, left, false, true, false, true);
}

bool PlayerLevel::CanLevelUp() {
    float castTimeCooldown = ScrollManager::GetLevelUpCooldown();
    auto now =RE::Calendar::GetSingleton()->GetHoursPassed();
    return lastLevelUp == 0 || now - lastLevelUp > castTimeCooldown;
}
