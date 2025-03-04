#include "Utils.h"
#include "Windows.h"

#undef GetObject

using _GetFormEditorID = const char* (*)(std::uint32_t);
std::string Utils::GetEditorId(const RE::TESForm* a_form) {
    //https://github.com/powerof3/CLibUtil/blob/master/include/CLIBUtil/editorID.hpp
    switch (a_form->GetFormType()) {
        case RE::FormType::Keyword:
        case RE::FormType::LocationRefType:
        case RE::FormType::Action:
        case RE::FormType::MenuIcon:
        case RE::FormType::Global:
        case RE::FormType::HeadPart:
        case RE::FormType::Race:
        case RE::FormType::Sound:
        case RE::FormType::Script:
        case RE::FormType::Navigation:
        case RE::FormType::Cell:
        case RE::FormType::WorldSpace:
        case RE::FormType::Land:
        case RE::FormType::NavMesh:
        case RE::FormType::Dialogue:
        case RE::FormType::Quest:
        case RE::FormType::Idle:
        case RE::FormType::AnimatedObject:
        case RE::FormType::ImageAdapter:
        case RE::FormType::VoiceType:
        case RE::FormType::Ragdoll:
        case RE::FormType::DefaultObject:
        case RE::FormType::MusicType:
        case RE::FormType::StoryManagerBranchNode:
        case RE::FormType::StoryManagerQuestNode:
        case RE::FormType::StoryManagerEventNode:
        case RE::FormType::SoundRecord:
            return a_form->GetFormEditorID();
        default: {
            static auto tweaks = GetModuleHandle(L"po3_Tweaks");
            static auto func = reinterpret_cast<_GetFormEditorID>(GetProcAddress(tweaks, "GetFormEditorID"));
            if (func) {
                return func(a_form->formID);
            }
            return {};
        }
    }
}

RE::BGSEquipSlot* Utils::GetSlot(bool left) {
    auto dom = RE::BGSDefaultObjectManager::GetSingleton();
    auto slot = dom->GetObject(left ? RE::DEFAULT_OBJECT::kLeftHandEquip : RE::DEFAULT_OBJECT::kRightHandEquip);
    // TODO: Null check?
    return slot->As<RE::BGSEquipSlot>();
}

RE::BGSEquipSlot* Utils::GetEitherHandSlot() {
    auto dom = RE::BGSDefaultObjectManager::GetSingleton();
    auto slot = dom->GetObject(RE::DEFAULT_OBJECT::kEitherHandEquip);
    // TODO: Null check?
    return slot->As<RE::BGSEquipSlot>();
}

std::string Utils::toLowerCase(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

static inline std::map<std::string, RE::ActorValue> avMap = {
    {"illusion", RE::ActorValue::kIllusion},
    {"restoration", RE::ActorValue::kRestoration},
    {"destruction", RE::ActorValue::kDestruction},
    {"alteration", RE::ActorValue::kAlteration},
    {"conjuration", RE::ActorValue::kConjuration},
};

RE::ActorValue Utils::ActorValueFromString(std::string& string) {
    auto lowerString = toLowerCase(string);
    auto it = avMap.find(lowerString);
    if (it != avMap.end()) {
        return it->second;
    }
    return RE::ActorValue::kNone;
}