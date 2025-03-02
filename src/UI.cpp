#include "UI.h"
#include "ScrollManager.h"

void UI::Register() {

    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection("Template");
    SKSEMenuFramework::AddSectionItem("Basic", Example1::Render);
}

void __stdcall UI::Example1::Render() {
    //ImGui::Text(std::format("Now: {}", RE::Calendar::GetSingleton()->GetHoursPassed()).c_str());

    for (auto & [key, value] : ScrollManager::GetTimesCastMap()) {
        ImGui::Text(std::format("Spell: {}, Times Cast: {}, Can Level Up: {}", key->GetName(), value->level, value->CanLevelUp()).c_str());
    }
}

