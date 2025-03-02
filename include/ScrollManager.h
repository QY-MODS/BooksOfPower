#pragma once
#include "Serializer.h"


struct HandBook {
    std::string weaponEditorId;
    RE::TESObjectWEAP* weapon = nullptr;
    RE::TESObjectWEAP* GetWeapon();
    void Equip(RE::Actor* actor);
};

struct PlayerLevel {
    uint32_t casts;
    float lastLevelUp = 0;
    bool CanLevelUp() {
        auto now = RE::Calendar::GetSingleton()->GetHoursPassed();
        return lastLevelUp == 0 || now - lastLevelUp > 0.01;
    }
};

struct ScrollData {
    RE::SpellItem* BaseSpell;
    RE::SpellItem* Scroll;
};

struct ScrollLevel {
    uint32_t casts;
    float magnitudePercentage;
    float durationPercentage;
    float costPercentage;
    uint32_t level;
};

using replaceModelMap = std::map<std::string, std::string>;
using handBooksMap = std::map<RE::ActorValue, HandBook*>;
using scrollDataMap = std::map<RE::SpellItem*, ScrollData*>;
using playerSkillMap = std::map<RE::SpellItem*, PlayerLevel*>;
using scrollLevelVector = std::vector<ScrollLevel*>;

class ScrollManager {


    static inline scrollLevelVector scrollLevels = {
        new ScrollLevel(0, 50, 50, 130, 1),
        new ScrollLevel(3, 80, 80, 120, 2),
        new ScrollLevel(7, 90, 90, 110, 3),
        new ScrollLevel(10, 10, 10, 100, 4),
    };

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
    static ScrollData* GetScrollData(RE::SpellItem* item);
    static PlayerLevel* GetPlayerSkill(RE::SpellItem* item);
    static ScrollLevel* GetScrollLevel(RE::SpellItem* level);
    static void ApplyLevel(RE::SpellItem* scroll);
    static void HandleLevelUp(RE::SpellItem* spell);

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