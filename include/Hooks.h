#pragma once
namespace Hooks {

    static inline RE::TESObjectWEAP* spellBook = nullptr;

    void Install();

    void DataLoaded();

    struct ScrollSpellTypeHook {
        static inline REL::Relocation<RE::MagicSystem::SpellType(RE::ScrollItem*)> originalFunction;
        static RE::MagicSystem::SpellType GetSpellType(RE::ScrollItem* ref);
        static void Install();
    };

    struct EquipObjectHook {
        static void Install();
        static void thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::TESBoundObject* a_object,
                          RE::ExtraDataList* a_extraData = nullptr, std::uint32_t a_count = 1,
                          const RE::BGSEquipSlot* a_slot = nullptr, bool a_queueEquip = true, bool a_forceEquip = false,
                          bool a_playSounds = true, bool a_applyNow = false);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    
    struct UnEquipObjectPCHook {
        static void Install();
        static void thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::TESBoundObject* a_object,
                          RE::ExtraDataList* a_extraData = nullptr, std::uint32_t a_count = 1,
                          const RE::BGSEquipSlot* a_slot = nullptr, bool a_queueEquip = true, bool a_forceEquip = false,
                          bool a_playSounds = true, bool a_applyNow = false,
                          const RE::BGSEquipSlot* a_slotToReplace = nullptr);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };


    // class GetActorValueForCost {
    // public:
    //     static void Install();

    // private:
    //     static RE::ActorValue thunk(RE::MagicItem* magicItem, bool rightHand);
    //     static inline REL::Relocation<decltype(thunk)> originalFunction;
    // };
}