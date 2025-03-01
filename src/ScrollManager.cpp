#include "ScrollManager.h"

RE::BGSEquipSlot* GetSlot(bool left) {
    auto dom = RE::BGSDefaultObjectManager::GetSingleton();
    auto slot = dom->GetObject(left ? RE::DEFAULT_OBJECT::kLeftHandEquip : RE::DEFAULT_OBJECT::kRightHandEquip);
    // TODO: Null check?
    return slot->As<RE::BGSEquipSlot>();
}

RE::TESForm* CreateFormByType(RE::FormType type) {
    using func_t = RE::TESForm*(RE::FormType);
    const REL::Relocation<func_t> func{RELOCATION_ID(13656, 13765)};
    auto result = func(type);
    result->SetFormID(result->GetFormID(), true);
    return result;
}

void ScrollManager::ReplaceSpellTome(RE::TESObjectBOOK* book) {
    auto spell = book->GetSpell();

    if (!spell) {
        return;
    }

    auto id = book->GetFormID();
    auto model = book->GetModel();
    auto name = book->GetName();
    auto value = book->GetGoldValue();
    auto weight = book->GetWeight();

    book->SetFormID(0, false);

    auto df = CreateFormByType(RE::FormType::Scroll);
    if (df) {
        if (auto newBook = df->As<RE::ScrollItem>()) {
            for (auto effect : spell->effects) {
                auto copy = new RE::Effect();
                copy->effectItem = effect->effectItem;

                copy->baseEffect = effect->baseEffect;
                copy->cost = effect->cost;
                copy->conditions = effect->conditions;

                newBook->effects.push_back(copy);
            }
            for (auto key : book->GetKeywords()) {
                newBook->AddKeyword(key);
            }
            newBook->SpellItem::data = spell->data;
            newBook->AddKeyword(keyword);
            newBook->SetModel(model);
            newBook->weight = weight;
            newBook->value = value;
            newBook->SetFullName(name);
            newBook->SetFormID(id, false);
        }
    }
}

void ScrollManager::DataLoaded() {
    spellBook = RE::TESForm::LookupByEditorID<RE::TESObjectWEAP>("BOP_SpellBook");
    keyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("BOP_ChannelingTome");

    auto scroll = RE::TESForm::LookupByEditorID<RE::ScrollItem>("BOP_FireballScroll");

    if (scroll) {
        const auto& [map, lock] = RE::TESForm::GetAllForms();
        const RE::BSReadWriteLock l{lock};
        for (auto& [id, form] : *map) {
            if (form) {
                if (auto book = form->As<RE::TESObjectBOOK>()) {
                    if (book->IsBookTome()) {
                        ReplaceSpellTome(book);
                    }
                }
            }
        }
    }
}

bool ScrollManager::OnEquip(RE::Actor* player, RE::TESBoundObject* a_object, RE::BGSEquipSlot** a_slot) {

    if (!a_object) {
        return true;
    }

    if (!player || !player->IsPlayerRef()) {
        return true;
    }

    if (DefaultBehavior) {
        return true;
    }

    auto leftHandItem = player->GetEquippedObject(true);
    auto rightHandItem = player->GetEquippedObject(false);
    auto left = GetSlot(true);
    auto right = GetSlot(false);

    if (leftHandItem == spellBook) {

        if (rightHandItem) {
            if (auto bound = rightHandItem->As<RE::TESBoundObject>()) {
                DefaultBehavior = true;
                RE::ActorEquipManager::GetSingleton()->UnequipObject(player, bound, nullptr, 1, right, false, true,
                                                                     false);
                DefaultBehavior = false;
            }
        }

    }

    if (rightHandItem && rightHandItem->HasKeywordByEditorID("BOP_ChannelingTome")) {

        if (auto bound = leftHandItem->As<RE::TESBoundObject>()) {
            DefaultBehavior = true;
            RE::ActorEquipManager::GetSingleton()->UnequipObject(player, bound, nullptr, 1, left, false, true,false);
            DefaultBehavior = false;
        }
    }

    if (a_object->HasKeywordByEditorID("BOP_ChannelingTome")) {

        *a_slot = right;
        if (left && spellBook) {
            auto inv = player->GetInventory();
            if (inv.find(spellBook) == inv.end()) {
                player->AddObjectToContainer(spellBook, nullptr, 1, nullptr);
            }
            RE::ActorEquipManager::GetSingleton()->EquipObject(player, spellBook, nullptr, 1, left, false, true, false,
                                                               true);
        }
    }
    return true;
}

bool ScrollManager::OnUnEquip(RE::Actor* player, RE::TESBoundObject* a_object, RE::BGSEquipSlot* a_slot) {

    if (!a_object) {
        return true;
    }

    if (!player || !player->IsPlayerRef()) {
        return true;
    }
    if (DefaultBehavior) {
        return true;
    }

    if (a_object->HasKeywordByEditorID("BOP_ChannelingTome")) {
        auto player = RE::PlayerCharacter::GetSingleton();
        auto slot = GetSlot(true);
        RE::ActorEquipManager::GetSingleton()->UnequipObject(player, spellBook, nullptr, 1, slot, false, true, false);
    }
    return true;
}
