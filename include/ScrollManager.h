#pragma once
#include "Serializer.h"


struct HandBook {
    std::string weaponEditorId;
    RE::TESObjectWEAP* weapon = nullptr;
    RE::TESObjectWEAP* GetWeapon();
    void Equip(RE::Actor* actor);
};

struct PlayerLevel {
    uint32_t level;
    float lastLevelUp = 0;
    bool CanLevelUp() {
        auto now = RE::Calendar::GetSingleton()->GetHoursPassed();
        return lastLevelUp == 0 || now - lastLevelUp > 0.01;
    }
};

struct ScrollData {
    RE::SpellItem* BaseSpell;
};

using replaceModelMap = std::map<std::string, std::string>;
using handBooksMap = std::map<RE::ActorValue, HandBook*>;
using scrollDataMap = std::map<RE::SpellItem*, ScrollData*>;
using playerSkillMap = std::map<RE::SpellItem*, PlayerLevel>;

class ScrollManager {

    static inline replaceModelMap replaceModels = {
        {"Clutter\\Books\\SpellTomeAlterationLowPoly.nif", "Books Of Power\\SpellTomeAlterationLowPoly.nif"},
        {"Clutter\\Books\\SpellTomeConjurationLowPoly.nif", "Books Of Power\\SpellTomeConjurationLowPoly.nif"},
        {"Clutter\\Books\\SpellTomeDestructionLowPoly.nif", "Books Of Power\\SpellTomeDestructionLowPoly.nif"},
        {"Clutter\\Books\\SpellTomeRestorationLowPoly.nif", "Books Of Power\\SpellTomeRestorationLowPoly.nif"},
        {"Clutter\\Books\\SpellTomeIllusionLowPoly.nif", "Books Of Power\\SpellTomeIllusionLowPoly.nif"},
    };

    static inline handBooksMap handBooks = {
        {RE::ActorValue::kIllusion, new HandBook("BOP_IllusionSpellBook")},
        {RE::ActorValue::kRestoration, new HandBook("BOP_RestorationSpellBook")},
        {RE::ActorValue::kDestruction, new HandBook("BOP_DestructionSpellBook")},
        {RE::ActorValue::kAlteration, new HandBook("BOP_AltlerationSpellBook")},
        {RE::ActorValue::kConjuration, new HandBook("BOP_ConjurationSpellBook")},
    };

    static inline scrollDataMap scrollData;
    static inline playerSkillMap playerSkill;

    static inline RE::BGSKeyword* keyword = nullptr;
    static inline bool DefaultBehavior = false;
    static void ReplaceSpellTome(RE::TESObjectBOOK* book);
    static ScrollData* GetScrollData(RE::ScrollItem* item);


public:
    static playerSkillMap& GetTimesCastMap();
    static void SaveGame(Serializer* serializer);
    static void LoadGame(Serializer* serializer);
    static void DataLoaded();
    static bool OnEquip(RE::Actor* player, RE::TESBoundObject* a_object, RE::BGSEquipSlot** a_slot);
    static bool OnUnEquip(RE::Actor* player, RE::TESBoundObject* a_object, RE::BGSEquipSlot* a_slot);
    static void OnCast(RE::Actor* caster, RE::SpellItem* spell);
    static void OnHit(RE::Actor* caster, RE::SpellItem* spell);
};