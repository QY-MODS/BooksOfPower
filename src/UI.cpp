#include "UI.h"

void UI::Register() {

    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection("Template");
    SKSEMenuFramework::AddSectionItem("Basic", Example1::Render);
}

void __stdcall UI::Example1::Render() {
    if (ImGui::Button("Test")) {
    
    }
}

