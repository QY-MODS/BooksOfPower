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
    uint32_t lastLevelCasts;
    float lastLevelUp = 0;
    bool CanLevelUp();
};

struct ScrollData {
    RE::SpellItem* BaseSpell;
    RE::SpellItem* Scroll;
    RE::TESObjectBOOK* OriginalItem;
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

    static inline uint32_t maxLevel = 1;
    static inline scrollLevelVector scrollLevels;
    static inline replaceModelMap replaceModels;
    static inline handBooksMap handBooks;

    static inline float castTimeCooldown = 0.01;
    static inline bool displayBookOnTheOtherHand = true;

    static inline scrollDataMap scrollData;
    static inline playerSkillMap playerSkill;

    static inline RE::BGSKeyword* keyword = nullptr;
    static inline RE::EffectSetting* costEffect = nullptr;
    static inline RE::EffectSetting* costPerSecoundEffect = nullptr;
    static inline RE::EffectSetting* levelEffect = nullptr;
    static inline bool DefaultBehavior = false;
    static ScrollData* GetScrollData(RE::SpellItem* item);
    static PlayerLevel* GetPlayerSkill(RE::SpellItem* item);
    static ScrollLevel* GetScrollLevel(RE::SpellItem* level);
    static void ApplyLevel(RE::SpellItem* scroll);
    static void HandleLevelUp(RE::SpellItem* spell);
    static void ReadConfigFile();
    static void ApplyEffectsToScroll(ScrollData* data);
public:
    static playerSkillMap& GetTimesCastMap();
    static void ReplaceBookWithScroll(RE::TESObjectBOOK* book);
    static float GetLevelUpCooldown();
    static void SaveGame(Serializer* serializer);
    static void LoadGame(Serializer* serializer);
    static void CleanLevel();
    static void DataLoaded();
    static bool OnEquip(RE::Actor* player, RE::TESBoundObject* a_object, RE::BGSEquipSlot** a_slot);
    static void OnUnEquip(RE::Actor* player, RE::TESBoundObject* a_object);
    static void OnCast(RE::Actor* caster, RE::SpellItem* spell);
};