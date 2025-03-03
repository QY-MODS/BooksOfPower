#include "Hooks.h"
#include "ScrollManager.h"

RE::MagicSystem::SpellType Hooks::ScrollSpellTypeHook::thunk(RE::ScrollItem* ref) {
    auto result = originalFunction(ref);

    if (ref && ref->HasKeywordByEditorID("BOP_ChannelingTome")) {
        return RE::MagicSystem::SpellType::kSpell;
    }

    return result;
}

void Hooks::ScrollSpellTypeHook::Install() {
    originalFunction = REL::Relocation<std::uintptr_t>(RE::ScrollItem::VTABLE[0]).write_vfunc(0x53, thunk);
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

RE::MagicSystem::CastingType Hooks::GetCastingTypeHook::thunk(RE::ScrollItem* ref) {
    if (ref && ref->HasKeywordByEditorID("BOP_ChannelingTome")) {
        return ref->SpellItem::data.castingType;
    }
    return originalFunction(ref);
}

void Hooks::GetCastingTypeHook::Install() {
    originalFunction = REL::Relocation<std::uintptr_t>(RE::ScrollItem::VTABLE[0]).write_vfunc(0x55, thunk);
}

RE::BSEventNotifyControl Hooks::SpellCastEvent::ProcessEvent(const RE::TESSpellCastEvent* event,
                                                             RE::BSTEventSource<RE::TESSpellCastEvent>*) {

    if (!event) {
        return RE::BSEventNotifyControl::kContinue;
    }

    if (auto obj = event->object) {
        if (auto form = obj.get()) {
            if (auto actor = form->As<RE::Actor>()) {
                if (actor->IsPlayerRef()) {
                    if (auto spell = RE::TESForm::LookupByID<RE::SpellItem>(event->spell)) {
                        ScrollManager::OnCast(actor, spell);
                    }
                }
            }
        }
    }

    return RE::BSEventNotifyControl::kContinue;
}

void Hooks::SpellCastEvent::Install() {
    RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink(new SpellCastEvent());
}

void Hooks::BookInitHook::thunk(RE::TESObjectBOOK* ref, RE::TESFile* file) {
    originalFunction(ref, file);
    ScrollManager::ReplaceBookWithScroll(ref);
}

void Hooks::BookInitHook::Install() {
    originalFunction = REL::Relocation<std::uintptr_t>(RE::TESObjectBOOK::VTABLE[0]).write_vfunc(0x6, thunk);
}


void Hooks::Install() {
    ScrollSpellTypeHook::Install();
    GetCastingTypeHook::Install();
    EquipObjectHook::Install();
    UnEquipObjectPCHook::Install();
    EquipSpellHook::Install();
    SpellCastEvent::Install();
    BookInitHook::Install();
}
