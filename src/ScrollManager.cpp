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

            skillActorValues[newBook] = spell->GetAssociatedSkill();
            newBook->SetFormID(id, false);

        }
    }
}

RE::ActorValue ScrollManager::GetSkill(RE::ScrollItem* item) { 
    auto it = skillActorValues.find(item);
    if (it != skillActorValues.end()) {
        return it->second;
    }
    return RE::ActorValue::kDestruction;
}

timesCastMap& ScrollManager::GetTimesCastMap() {
    return timesCast;
}

void ScrollManager::SaveGame(Serializer* serializer) {}

void ScrollManager::LoadGame(Serializer* serializer) {
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

            auto it = handBooks.find(GetSkill(scroll));
            if (it != handBooks.end()) {
                it->second->Equip(player);
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
            auto now = RE::Calendar::GetSingleton()->GetHoursPassed();
            if (timesCast[spell].CanLevelUp()) {
                timesCast[spell].level++;
                timesCast[spell].lastLevelUp=now;
            }
        }
    }

}

void ScrollManager::OnHit(RE::Actor* caster, RE::SpellItem* spell) {
    if (spell->HasKeywordByEditorID("BOP_ChannelingTome")) {
        auto now = RE::Calendar::GetSingleton()->GetHoursPassed();
        if (timesCast[spell].CanLevelUp()) {
            timesCast[spell].level++;
            timesCast[spell].lastLevelUp = now;
        }
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
