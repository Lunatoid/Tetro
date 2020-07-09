#include "GameMasterScene.h"

#include "GameScene.h"

GameMasterScene::GameMasterScene(App* app, StringView name, int boardCount, JsonValue* config) :
    Scene(app, name), boardCount(boardCount), boardManager(app) {

    f64 bgmVolume;
    if (config->TryGetNumber("bgm_volume", bgmVolume)) {
        bgm.volume = (f32)bgmVolume;
    }

    f64 configMinSwitchTime;
    if (config->TryGetNumber("min_switch_time", configMinSwitchTime)) {
        minSwitchTime = (f32)configMinSwitchTime;
    }

    f64 configMaxSwitchTime;
    if (config->TryGetNumber("max_switch_time", configMaxSwitchTime)) {
        maxSwitchTime = (f32)configMaxSwitchTime;
    }

    BoundedString<8> boardName;
    for (int i = 0; i < boardCount; i++) {
        boardName.Format("{}", i);
        boardManager.PushScene<GameScene>(boardName, config);
    }

    if (boardCount > 1) {
        currentBoard = RandomRange(0, boardCount);

        boardName.Format("{}", currentBoard);
        boardManager.SetCurrent(boardName);

        nextSwitchTime = (f32)RandomRange(minSwitchTime, maxSwitchTime);
    } else {
        boardManager.SetCurrent("0");
    }

    bgm.Load("GameBGM");
    bgm.SetLooping(true);
    bgm.Play();
}

void GameMasterScene::OnFocus() {
    bgm.Resume();
}

void GameMasterScene::OnUnfocus() {
    bgm.Pause();
}

void GameMasterScene::OnUpdate(f32 dt) {
    if (boardCount > 1) {
        nextSwitchTime -= dt;

        if (nextSwitchTime < 0.0f) {
            // Time to switch!
            int newBoard = 0;
            do {
                newBoard = RandomRange(0, boardCount);
            } while (newBoard == currentBoard);

            currentBoard = newBoard;

            BoundedString<8> boardName;
            boardName.Format("{}", currentBoard);
            boardManager.SetCurrent(boardName);

            nextSwitchTime = (f32)RandomRange(minSwitchTime, maxSwitchTime);
        }
    }

    if (input.IsKeyPressed(Key::Escape)) {
        sceneManager.SetCurrent("Pause");
    }

    boardManager.Update(dt);
}

void GameMasterScene::OnRender(f32 dt) {
    boardManager.Render(dt);
}
