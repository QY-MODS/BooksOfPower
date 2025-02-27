#pragma once


class ScrollManager {
    static inline RE::TESObjectWEAP* spellBook = nullptr;
    static inline bool DefaultBehavior = false;
public:
    static void DataLoaded();
    static bool OnEquip(RE::Actor* player, RE::TESBoundObject* a_object, RE::BGSEquipSlot** a_slot);
    static bool OnUnEquip(RE::Actor* player, RE::TESBoundObject* a_object, RE::BGSEquipSlot* a_slot);
};