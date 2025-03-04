#pragma once

namespace Utils {

    std::string GetEditorId(const RE::TESForm* a_form);

	RE::BGSEquipSlot* GetSlot(bool left);
    RE::BGSEquipSlot* GetEitherHandSlot();
    std::string toLowerCase(const std::string& input);
    RE::ActorValue ActorValueFromString(std::string& string);

    template <class T>
    inline T* CreateFormByType() {
        using func_t = RE::TESForm*(RE::FormType);
        const REL::Relocation<func_t> func{RELOCATION_ID(13656, 13765)};
        auto result = func(T::FORMTYPE);
        // result->SetFormID(result->GetFormID(), true); //this is also done in the game code but is not desired in the
        // mod
        if (!result) {
            return nullptr;
        }
        return result->As<T>();
    }


}