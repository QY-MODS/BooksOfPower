#pragma once

namespace Utils {

	inline RE::BGSEquipSlot* GetSlot(bool left) {
        auto dom = RE::BGSDefaultObjectManager::GetSingleton();
        auto slot = dom->GetObject(left ? RE::DEFAULT_OBJECT::kLeftHandEquip : RE::DEFAULT_OBJECT::kRightHandEquip);
        // TODO: Null check?
        return slot->As<RE::BGSEquipSlot>();
    }
    inline RE::BGSEquipSlot* GetEitherHandSlot() {
        auto dom = RE::BGSDefaultObjectManager::GetSingleton();
        auto slot = dom->GetObject(RE::DEFAULT_OBJECT::kEitherHandEquip);
        // TODO: Null check?
        return slot->As<RE::BGSEquipSlot>();
    }

    inline RE::TESForm* CreateFormByType(RE::FormType type) {
        using func_t = RE::TESForm*(RE::FormType);
        const REL::Relocation<func_t> func{RELOCATION_ID(13656, 13765)};
        auto result = func(type);
        // result->SetFormID(result->GetFormID(), true);
        return result;
    }
    static inline std::map<std::string, RE::ActorValue> avMap = {
        {"illusion", RE::ActorValue::kIllusion},
        {"restoration", RE::ActorValue::kRestoration},
        {"destruction", RE::ActorValue::kDestruction},
        {"alteration", RE::ActorValue::kAlteration},
        {"conjuration", RE::ActorValue::kConjuration},
    };
    static inline std::string toLowerCase(const std::string& input) {
        std::string result = input;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    inline RE::ActorValue ActorValueFromString(std::string& string) {
        auto lowerString = toLowerCase(string);
        auto it = avMap.find(lowerString);
        if (it != avMap.end()) {
            return it->second;
        }
        return RE::ActorValue::kNone;
    }

}