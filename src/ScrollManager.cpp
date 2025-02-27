#include "ScrollManager.h"

RE::BGSEquipSlot* GetSlot(bool left) {
    auto dom = RE::BGSDefaultObjectManager::GetSingleton();
    auto slot = dom->GetObject(left ? RE::DEFAULT_OBJECT::kLeftHandEquip : RE::DEFAULT_OBJECT::kRightHandEquip);
    // TODO: Null check?
    return slot->As<RE::BGSEquipSlot>();
}

void ScrollManager::DataLoaded() {
    spellBook = RE::TESForm::LookupByEditorID<RE::TESObjectWEAP>("BOP_SpellBook");
    if (spellBook) {
        logger::trace("Name: {}", spellBook->GetName());
    } else {
        logger::trace("aa");
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
    auto left = GetSlot(true);
    auto right = GetSlot(false);

    if (leftHandItem == spellBook) {

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

        if (auto bound = leftHandItem->As<RE::TESBoundObject>()) {
            DefaultBehavior = true;
            RE::ActorEquipManager::GetSingleton()->UnequipObject(player, bound, nullptr, 1, left, false, true,false);
            DefaultBehavior = false;
        }
    }

    if (a_object->HasKeywordByEditorID("BOP_ChannelingTome")) {

        *a_slot = right;
        if (left && spellBook) {
            auto inv = player->GetInventory();
            if (inv.find(spellBook) == inv.end()) {
                player->AddObjectToContainer(spellBook, nullptr, 1, nullptr);
            }
            RE::ActorEquipManager::GetSingleton()->EquipObject(player, spellBook, nullptr, 1, left, false, true, false,
                                                               true);
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
        auto slot = GetSlot(true);
        RE::ActorEquipManager::GetSingleton()->UnequipObject(player, spellBook, nullptr, 1, slot, false, true, false);
    }
    return true;
}
