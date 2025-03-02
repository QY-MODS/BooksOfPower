#include "Persistence.h"
#include "Serializer.h"
#include "ScrollManager.h"
#define LATEST_VERSION 2


static void SaveCallback(SKSE::SerializationInterface* a_intfc) {
    if (a_intfc->OpenRecord('SPEL', LATEST_VERSION)) {

        auto serializer = new Serializer(a_intfc);
        ScrollManager::SaveGame(serializer);
        delete serializer;

    }
}


static void LoadCallback(SKSE::SerializationInterface* a_intfc) {
    uint32_t type;
    uint32_t version;
    uint32_t length;
    bool refreshGame = false;

    while (a_intfc->GetNextRecordInfo(type, version, length)) {
        if (type == 'SPEL' && version == LATEST_VERSION) {

            auto serializer = new Serializer(a_intfc);
            ScrollManager::LoadGame(serializer);
            delete serializer;

        }
    }
}

void Persistence::Install() {
    auto serialization = SKSE::GetSerializationInterface();
    serialization->SetUniqueID('BOKP');
    serialization->SetSaveCallback(SaveCallback);
    serialization->SetLoadCallback(LoadCallback);
}


