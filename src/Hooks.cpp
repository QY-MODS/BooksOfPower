#include "Hooks.h"

#include "Hooks.h"

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
}

RE::MagicSystem::SpellType Hooks::ScrollSpellTypeHook::GetSpellType(RE::ScrollItem* ref) {
    auto result = originalFunction(ref);

    if (ref->HasKeywordByEditorID("BOP_InfiniteScroll")) {
        return RE::MagicSystem::SpellType::kSpell;
    
    }
    return result;
}

void Hooks::ScrollSpellTypeHook::Install() {
    originalFunction = REL::Relocation<std::uintptr_t>(RE::ScrollItem::VTABLE[0]).write_vfunc(0x53, GetSpellType);
}
