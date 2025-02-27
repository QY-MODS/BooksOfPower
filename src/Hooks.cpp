#include "Hooks.h"
#include "ScrollManager.h"

// void Hooks::GetActorValueForCost::Install() {
//     SKSE::AllocTrampoline(14);
//     auto& trampoline = SKSE::GetTrampoline();
//     originalFunction = trampoline.write_call<5>(REL::RelocationID(33362, 34143).address() + REL::Relocate(0x151,
//     0x151),
//                                                 thunk);  // Drain
//     trampoline.write_call<5>(REL::RelocationID(33359, 34140).address() + REL::Relocate(0x4c, 0x4d),
//                              thunk);  // Cancel  cast
//     trampoline.write_call<5>(REL::RelocationID(33364, 34145).address() + REL::Relocate(0xc1, 0xbe), thunk);  // Can
//     cast
// }
//
// RE::ActorValue Hooks::GetActorValueForCost::thunk(RE::MagicItem* magicItem, bool rightHand) {
//
//     auto av = originalFunction(magicItem, rightHand);
//
//     logger::trace("{}", av);
//
//     return RE::ActorValue::kMagicka;
// }

void Hooks::Install() {
    // GetActorValueForCost::Install();
    ScrollSpellTypeHook::Install();
    EquipObjectHook::Install();
    UnEquipObjectPCHook::Install();
}



RE::MagicSystem::SpellType Hooks::ScrollSpellTypeHook::GetSpellType(RE::ScrollItem* ref) {
    auto result = originalFunction(ref);

    if (ref->HasKeywordByEditorID("BOP_ChannelingTome")) {
        return RE::MagicSystem::SpellType::kSpell;
    
    }
    return result;
}

void Hooks::ScrollSpellTypeHook::Install() {
    originalFunction = REL::Relocation<std::uintptr_t>(RE::ScrollItem::VTABLE[0]).write_vfunc(0x53, GetSpellType);
}

void Hooks::EquipObjectHook::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(REL::RelocationID(37951, 38907).address() + REL::Relocate(0x2e0, 0x2e0), thunk);
}



void Hooks::EquipObjectHook::thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::TESBoundObject* a_object,
                                   RE::ExtraDataList* a_extraData, std::uint32_t a_count,
                                   const RE::BGSEquipSlot* a_slot, bool a_queueEquip, bool a_forceEquip,
                                   bool a_playSounds, bool a_applyNow) {

    if (ScrollManager::OnEquip(a_actor, a_object, &a_slot)) {
        originalFunction(a_manager, a_actor, a_object, a_extraData, a_count, a_slot, a_queueEquip, a_forceEquip,
                         a_playSounds, a_applyNow);
    }


}

void Hooks::UnEquipObjectPCHook::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction =
        trampoline.write_call<5>(REL::RelocationID(37951, 38907).address() + REL::Relocate(0x2a9, 0x2a9), thunk);
}

void Hooks::UnEquipObjectPCHook::thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor,
                                       RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraData,
                                       std::uint32_t a_count, const RE::BGSEquipSlot* a_slot, bool a_queueEquip,
                                       bool a_forceEquip, bool a_playSounds, bool a_applyNow,
                                       const RE::BGSEquipSlot* a_slotToReplace) {

    if (ScrollManager::OnUnEquip(a_actor, a_object, a_slot)) {
        originalFunction(a_manager, a_actor, a_object, a_extraData, a_count, a_slot, a_queueEquip, a_forceEquip,
                            a_playSounds, a_applyNow, a_slotToReplace);
    }
}
