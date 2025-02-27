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
    EquipSpellHook::Install();
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
                                   RE::BGSEquipSlot* a_slot, bool a_queueEquip, bool a_forceEquip,
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
                                       std::uint32_t a_count, RE::BGSEquipSlot* a_slot, bool a_queueEquip,
                                       bool a_forceEquip, bool a_playSounds, bool a_applyNow,
                                       RE::BGSEquipSlot* a_slotToReplace) {

    if (ScrollManager::OnUnEquip(a_actor, a_object, a_slot)) {
        originalFunction(a_manager, a_actor, a_object, a_extraData, a_count, a_slot, a_queueEquip, a_forceEquip,
                            a_playSounds, a_applyNow, a_slotToReplace);
    }
}




void Hooks::EquipSpellHook::thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                                  RE::BGSEquipSlot** a_slot_ptr) {
    if (ScrollManager::OnEquip(a_actor, a_spell, a_slot_ptr)) {
        originalFunction(a_manager, a_actor, a_spell, a_slot_ptr);
    }
}

void Hooks::EquipSpellHook::Install() {
    // SE ID: 37952 SE Offset: 0xd7 (Heuristic)
    // AE ID: 38908 AE Offset: 0xd7
    //
    // SE ID: 37950 SE Offset: 0xc5 (Heuristic)
    // AE ID: 38906 AE Offset: 0xca
    //
    // SE ID: 37939 SE Offset: 0x47 (Heuristic)
    // AE ID: 38895 AE Offset: 0x47
    SKSE::AllocTrampoline(14 * 3);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction =
        trampoline.write_call<5>(REL::RelocationID(37952, 38908).address() + REL::Relocate(0xd7, 0xd7),  // Click
                                 thunk);                                                                 // Clicking
    // trampoline.write_call<5>(REL::RelocationID(37950, 38906).address() + REL::Relocate(0xc5, 0xca), thunk); // Hotkey
    // trampoline.write_call<5>(REL::RelocationID(37939, 38895).address() + REL::Relocate(0x47, 0x47), thunkPresise); //
    // Commonlib
}