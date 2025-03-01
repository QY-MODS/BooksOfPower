#pragma once

namespace Utils {

	inline RE::BGSEquipSlot* GetSlot(bool left) {
        auto dom = RE::BGSDefaultObjectManager::GetSingleton();
        auto slot = dom->GetObject(left ? RE::DEFAULT_OBJECT::kLeftHandEquip : RE::DEFAULT_OBJECT::kRightHandEquip);
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

}