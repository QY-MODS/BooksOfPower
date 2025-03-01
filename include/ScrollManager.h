#pragma once

class ScrollManager {

    static inline std::map<std::string, std::string> replaceModels = {
        {"Clutter\\Books\\SpellTomeAlterationLowPoly.nif", "Books Of Power\\SpellTomeAlterationLowPoly.nif"},
        {"Clutter\\Books\\SpellTomeConjurationLowPoly.nif", "Books Of Power\\SpellTomeConjurationLowPoly.nif"},
        {"Clutter\\Books\\SpellTomeAlterationLowPoly.nif", "Books Of Power\\SpellTomeDestructionLowPoly.nif"},
        {"Clutter\\Books\\SpellTomeAlterationLowPoly.nif", "Books Of Power\\SpellTomeRestorationLowPoly.nif"},
        {"Clutter\\Books\\SpellTomeAlterationLowPoly.nif", "Books Of Power\\SpellTomeIllusionLowPoly.nif"},
    };

    static inline RE::TESObjectWEAP* spellBook = nullptr;
    static inline RE::BGSKeyword* keyword = nullptr;
    static inline bool DefaultBehavior = false;
    static void ReplaceSpellTome(RE::TESObjectBOOK* book);

public:
    static void DataLoaded();
    static bool OnEquip(RE::Actor* player, RE::TESBoundObject* a_object, RE::BGSEquipSlot** a_slot);
    static bool OnUnEquip(RE::Actor* player, RE::TESBoundObject* a_object, RE::BGSEquipSlot* a_slot);
};