#include "ScrollManager.h"
#include "Utils.h"

void ScrollManager::ReplaceSpellTome(RE::TESObjectBOOK* book) {
    auto spell = book->GetSpell();

    if (!spell) {
        return;
    }

    auto id = book->GetFormID();
    auto model = book->GetModel();
    auto name = book->GetName();
    auto value = book->GetGoldValue();
    auto weight = book->GetWeight();


    auto df = Utils::CreateFormByType(RE::FormType::Scroll);
    if (df) {
        if (auto newBook = df->As<RE::ScrollItem>()) {

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

            scrollData[newBook] = new ScrollData(spell, newBook);
            newBook->SetFormID(id, false);
            ApplyLevel(newBook);
        }
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
    return nullptr;
}

void ScrollManager::ApplyLevel(RE::SpellItem* scroll) {
    auto data = GetScrollData(scroll);
    if (data) {
        auto level = GetScrollLevel(scroll);
        if (level) {
            auto base = data->BaseSpell;
            auto scroll = data->Scroll;
            scroll->data.costOverride = base->data.costOverride * level->costPercentage / 100;
            if (base->effects.size() == scroll->effects.size()) {
                for (auto i = 0; i < base->effects.size(); i++) {
                    scroll->effects[i]->effectItem.magnitude = base->effects[i]->effectItem.magnitude*level->magnitudePercentage/100;
                    scroll->effects[i]->effectItem.duration = base->effects[i]->effectItem.duration*level->durationPercentage/100;
                    scroll->effects[i]->cost = base->effects[i]->cost *level->costPercentage/100;
                }
            }
        } else {
            auto base = data->BaseSpell;
            auto scroll = data->Scroll;
            scroll->data.costOverride = base->data.costOverride;
            if (base->effects.size() == scroll->effects.size()) {
                for (auto i = 0; i < base->effects.size(); i++) {
                    scroll->effects[i]->effectItem.magnitude = base->effects[i]->effectItem.magnitude;
                    scroll->effects[i]->effectItem.duration = base->effects[i]->effectItem.duration;
                    scroll->effects[i]->cost = base->effects[i]->cost;
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

    if (playerSkill[spell]->CanLevelUp()) {
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
                        RE::DebugNotification(std::format("Your knowledge at the spell {} is at level {}", data->BaseSpell->GetName(), level->level).c_str());
                    }
                } 
                else 
                {
                    auto player = RE::PlayerCharacter::GetSingleton();
                    if (!player->HasSpell(data->BaseSpell)) {
                        player->AddSpell(data->BaseSpell);
                        RE::DebugNotification(std::format("You mastered the spell {}",data->BaseSpell->GetName()).c_str());
                    }
                }
        }
    }
}

playerSkillMap& ScrollManager::GetTimesCastMap() {
    return playerSkill;
}

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

    for (auto [key, value] : playerSkill) {
        delete value;
    }

    playerSkill.clear();

    for (uint32_t i = 0; i < length; i++) 
    {
        auto form = serializer->ReadForm<RE::SpellItem>();
        auto level = serializer->Read<uint32_t>();
        auto lastLevelCasts = serializer->Read<uint32_t>();
        playerSkill[form] = new PlayerLevel(level,lastLevelCasts);
        ApplyLevel(form);
    }
}

void ScrollManager::DataLoaded() {
    keyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("BOP_ChannelingTome");

    auto scroll = RE::TESForm::LookupByEditorID<RE::ScrollItem>("BOP_FireballScroll");

    if (scroll) {
        const auto& [map, lock] = RE::TESForm::GetAllForms();
        const RE::BSReadWriteLock l{lock};
        for (auto& [id, form] : *map) {
            if (form) {
                if (auto book = form->As<RE::TESObjectBOOK>()) {
                    if (book->IsBookTome()) {
                        ReplaceSpellTome(book);
                    }
                }
            }
        }
    }
}

bool ScrollManager::OnEquip(RE::Actor* player, RE::TESBoundObject* a_object, RE::BGSEquipSlot** a_slot) {

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

        *a_slot = right;
        if (auto scroll = a_object->As<RE::ScrollItem>()) {

            auto data = GetScrollData(scroll);
            if (data) {
                auto it = handBooks.find(data->BaseSpell->GetAssociatedSkill());
                if (it != handBooks.end()) {
                    it->second->Equip(player);
                }
            }
        }
    }
    return true;
}

bool ScrollManager::OnUnEquip(RE::Actor* player, RE::TESBoundObject* a_object, RE::BGSEquipSlot* a_slot) {

    if (!a_object) {
        return true;
    }

    if (!player || !player->IsPlayerRef()) {
        return true;
    }
    if (DefaultBehavior) {
        return true;
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
    return true;
}

void ScrollManager::OnCast(RE::Actor* caster, RE::SpellItem* spell) {
    if (spell->GetDelivery() == RE::MagicSystem::Delivery::kSelf) {
        if (spell->HasKeywordByEditorID("BOP_ChannelingTome")) {
            HandleLevelUp(spell);
        }
    }

}

void ScrollManager::OnHit(RE::Actor* caster, RE::SpellItem* spell) {
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
