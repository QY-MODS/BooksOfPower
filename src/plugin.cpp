#include "Plugin.h"
#include "Hooks.h"
#include "ScrollManager.h"
#include "Persistence.h"

void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        ScrollManager::DataLoaded();
    }
    if (message->type == SKSE::MessagingInterface::kNewGame) {
        ScrollManager::CleanLevel();
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    SetupLog();
    logger::info("Plugin loaded");
    UI::Register();
    Hooks::Install();
    Persistence::Install();
    return true;
}