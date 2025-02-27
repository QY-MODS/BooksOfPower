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

bool ScrollManager::OnEquip(RE::Actor* player, RE::TESBoundObject* a_object, const RE::BGSEquipSlot** a_slot) {

    if (!player || !player->IsPlayerRef()) {
        return true;
    }

    if (RemoveBook) {
        return true;
    }
    auto equipped = player->GetEquippedObject(true);
    auto scroll = player->GetEquippedObject(false);
    auto left = GetSlot(true);
    auto right = GetSlot(false);

    if (equipped == spellBook && *a_slot == left) {

        RemoveBook = true;
        if (scroll) {
            if (auto bound = scroll->As<RE::TESBoundObject>()) {
                RE::ActorEquipManager::GetSingleton()->UnequipObject(player, bound, nullptr, 1, right, false, true, false);
            }
        }
        RemoveBook = false;

        return true;
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
    if (GetSlot(true) != *a_slot || equipped != spellBook) {
        if (equipped && equipped->HasKeywordByEditorID("BOP_ChannelingTome")) {
            RemoveBook = true;
            RE::ActorEquipManager::GetSingleton()->UnequipObject(player, spellBook, nullptr, 1, left, false, true,
                                                                 false);
            RemoveBook = false;
        }
        return true;
    }
    return false;

}

bool ScrollManager::OnUnEquip(RE::Actor* player, RE::TESBoundObject* a_object, const RE::BGSEquipSlot* a_slot) {
    if (!player || !player->IsPlayerRef()) {
        return true;
    }

    if (a_object->HasKeywordByEditorID("BOP_ChannelingTome")) {
        auto player = RE::PlayerCharacter::GetSingleton();
        auto slot = GetSlot(true);
        RE::ActorEquipManager::GetSingleton()->UnequipObject(player, spellBook, nullptr, 1, slot, false, true, false);
    }
    return true;
}
