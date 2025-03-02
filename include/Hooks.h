#pragma once
namespace Hooks {



    void Install();


    struct ScrollSpellTypeHook {
        static inline REL::Relocation<RE::MagicSystem::SpellType(RE::ScrollItem*)> originalFunction;
        static RE::MagicSystem::SpellType GetSpellType(RE::ScrollItem* ref);
        static void Install();
    };

    class SpellCastEvent : public RE::BSTEventSink<RE::TESSpellCastEvent> {
        RE::BSEventNotifyControl ProcessEvent(const RE::TESSpellCastEvent* event,
                                              RE::BSTEventSource<RE::TESSpellCastEvent>*);

    public:
        static void Install();
    };

    class HitEvent : public RE::BSTEventSink<RE::TESHitEvent> {
        RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* event,
                                              RE::BSTEventSource<RE::TESHitEvent>*);

    public:
        static void Install();
    };


    struct GetCastingTypeHook {
        static inline REL::Relocation<RE::MagicSystem::CastingType(RE::ScrollItem*)> originalFunction;
        static RE::MagicSystem::CastingType GetCastingType(RE::ScrollItem* ref);
        static void Install();
    };

    struct GetChargeTimeHook {
        static inline REL::Relocation<float(RE::SpellItem*)> originalFunction;
        static float GetChargeTime(RE::ScrollItem* ref);
        static void Install();
    };

    struct EquipObjectHook {
        static void Install();
        static void thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::TESBoundObject* a_object,
                          RE::ExtraDataList* a_extraData = nullptr, std::uint32_t a_count = 1,
                          RE::BGSEquipSlot* a_slot = nullptr, bool a_queueEquip = true, bool a_forceEquip = false,
                          bool a_playSounds = true, bool a_applyNow = false);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    struct EquipSpellHook {
        static void thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                          RE::BGSEquipSlot** a_slot);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static void Install();
    };


    
    struct UnEquipObjectPCHook {
        static void Install();
        static void thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::TESBoundObject* a_object,
                          RE::ExtraDataList* a_extraData, std::uint32_t a_count, RE::BGSEquipSlot* a_slot,
                          bool a_queueEquip, bool a_forceEquip, bool a_playSounds, bool a_applyNow,
                          RE::BGSEquipSlot* a_slotToReplace);
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