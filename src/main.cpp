
#include <Pandora/Entry.h>

#include "MenuScene.h"
#include "PauseScene.h"

using namespace pd;

class Game : public App {
public:
    Game(int argc, char** argv, VideoBackend backend, JsonValue* config) :
        App(argc, argv, backend, "Tetro", Vec2i(580, 720)), config(config) {
        console.SetVisible(true);

        catalog.Load("Tetro.box");

        InitStorage("TomMol", "Tetro");

        FileStream stream;
        if (CreateStorageFile("ThirdPartyLicenses.txt", stream)) {
            Ref<BinaryResource> licenses = catalog.Get<BinaryResource>("Licenses");
            stream.WriteBytes(licenses->GetData());
            stream.Close();
        }

        sceneManager.PushScene<MenuScene>("Menu", config);
        sceneManager.PushScene<PauseScene>("Pause");
        sceneManager.SetCurrent("Menu");
    }

    ~Game() {
        Delete(config);
    }

private:
    JsonValue* config = nullptr;
};

App* pd::CreateApp(int argc, char** argv) {
    VideoBackend backend = VideoBackend::DirectX;

    JsonParseSettings settings;
    settings.allowComments = true;

    JsonValue* json = New<JsonValue>();
    if (json->Parse("config.json", true, settings) && json->Type() == JsonType::Object && json->HasField("backend")) {
        JsonValue back = json->GetField("backend");
        if (back.Type() == JsonType::String) {
            for (int i = 0; i < (int)VideoBackend::Count; i++) {

                if (back.GetString() == pd::VIDEO_BACKEND_NAMES[i]) {
                    backend = (VideoBackend)i;
                    break;
                }
            }
        }
    }

    return New<Game>(argc, argv, backend, json);
}
