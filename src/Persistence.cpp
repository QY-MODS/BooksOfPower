#include "Persistence.h"
#include "Serializer.h"

#define LATEST_VERSION 1


static void SaveCallback(SKSE::SerializationInterface* a_intfc) {
    if (a_intfc->OpenRecord('SPEL', LATEST_VERSION)) {

        auto serializer = Serializer(a_intfc);

    }
}


static void LoadCallback(SKSE::SerializationInterface* a_intfc) {
    uint32_t type;
    uint32_t version;
    uint32_t length;
    bool refreshGame = false;

    while (a_intfc->GetNextRecordInfo(type, version, length)) {
        if (type == 'SPEL' && version == LATEST_VERSION) {

            auto serializer = Serializer(a_intfc);


        }
    }
}

void Persistence::Install() {
    auto serialization = SKSE::GetSerializationInterface();
    serialization->SetUniqueID('DSTV');
    serialization->SetSaveCallback(SaveCallback);
    serialization->SetLoadCallback(LoadCallback);
}


