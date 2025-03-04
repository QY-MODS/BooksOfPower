#pragma once
namespace Hooks {

    void Install();

    struct ScrollSpellTypeHook {
        static RE::MagicSystem::SpellType thunk(RE::ScrollItem* ref);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static void Install();
    };

    class SpellCastEvent : public RE::BSTEventSink<RE::TESSpellCastEvent> {
        RE::BSEventNotifyControl ProcessEvent(const RE::TESSpellCastEvent* event,
                                              RE::BSTEventSource<RE::TESSpellCastEvent>*);
    public:
        static void Install();
    };

    struct GetCastingTypeHook {
        static RE::MagicSystem::CastingType thunk(RE::ScrollItem* ref);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
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


    struct BookInitHook {
        static void thunk(RE::TESObjectBOOK* ref, RE::TESFile* file);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static void Install();
    };

    struct RemoveItemHook {
        static RE::ObjectRefHandle thunk(int64_t a_unk, RE::TESBoundObject* a_item,
                                         std::int32_t a_count,
                                         RE::ITEM_REMOVE_REASON a_reason,
                          RE::ExtraDataList* a_extraList, RE::TESObjectREFR* a_moveToRef,
                          const RE::NiPoint3* a_dropLoc = 0, const RE::NiPoint3* a_rotate = 0);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static void Install();
    };
}