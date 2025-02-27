#pragma once


class ScrollManager {
    static inline RE::TESObjectWEAP* spellBook = nullptr;
    static inline bool RemoveBook = false;
public:
    static void DataLoaded();
    static bool OnEquip(RE::Actor* player, RE::TESBoundObject* a_object, const RE::BGSEquipSlot** a_slot);
    static bool OnUnEquip(RE::Actor* player, RE::TESBoundObject* a_object, const RE::BGSEquipSlot* a_slot);
};