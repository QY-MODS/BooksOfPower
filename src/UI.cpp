#include "UI.h"
#include "ScrollManager.h"

void UI::Register() {

    #ifndef NDEBUG

        if (!SKSEMenuFramework::IsInstalled()) {
            return;
        }
        SKSEMenuFramework::SetSection("Books Of Power");
        SKSEMenuFramework::AddSectionItem("Debug", Example1::Render);

    #endif
}

void __stdcall UI::Example1::Render() {
    //ImGui::Text(std::format("Now: {}", RE::Calendar::GetSingleton()->GetHoursPassed()).c_str());

    for (auto & [key, value] : ScrollManager::GetTimesCastMap()) {
        ImGui::Text(std::format("Spell: {}, Times Cast: {}, Can Level Up: {}", key->GetName(), value->casts, value->CanLevelUp()).c_str());
    }
}

