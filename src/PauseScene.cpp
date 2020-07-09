#include "PauseScene.h"

#include "Pandora/App.h"

PauseScene::PauseScene(App* app, StringView name) : Scene(app, name) {

    auto addOption = [&](StringView name) {
        options.Add();
        options.Last().text.Set(name);
        options.Last().scale = Vec2(0.5f);
        options.Last().Load("GuiFont");
        options.Last().GenerateSprites();
    };

    addOption("Continue");
    addOption("Main Menu");
    addOption("\nQuit");

    selectSfx.Load("SFX/Drop");
    confirmSfx.Load("SFX/Clear");

    bg.Load("MenuBG");

    Vec2i size = window.GetSize();
    optionOffset.x = size.x / 9.0f;
    optionOffset.y = size.y / 2.0f;
    cam.Init(size);
    cam.position.xy += (Vec2)size / 2.0f;
}

void PauseScene::OnFocus() {
    index = 0;
}

void PauseScene::OnUpdate(f32 dt) {
    if (input.IsKeyPressed(Key::Up)) {
        selectSfx.Play();

        if (index == 0) {
            index = options.Count() - 1;
        } else {
            index -= 1;
        }
    }

    if (input.IsKeyPressed(Key::Down)) {
        selectSfx.Play();

        if (index == options.Count() - 1) {
            index = 0;
        } else {
            index += 1;
        }
    }

    if (input.IsKeyPressed(Key::Space)) {
        confirmSfx.Play();

        // Ew
        switch (index) {
            case 0:
                sceneManager.SetCurrent("Game");
                break;

            case 1:
                sceneManager.RemoveScene("Game");
                sceneManager.SetCurrent("Menu");
                break;

            case 2:
                app->Quit();
                break;
        }
    }

    f32 nextOffset = 0.0f;
    for (int i = 0; i < options.Count(); i++) {
        options[i].position = Vec2(optionOffset - Vec2(0.0f, nextOffset));
        options[i].color = (i == index) ? Color(1.0f, 1.0f, 0.0f, 1.0f) : Color(1.0f);
        options[i].UpdateProperties();

        nextOffset += options[i].CalculateBounds().y * 1.1f;
    }

    cam.UpdateProjection();
}

void PauseScene::OnRender(f32 dt) {
    spriteRenderer.UpdateProjection(cam.GetMatrix());

    for (int i = 0; i < options.Count(); i++) {
        options[i].Draw(spriteRenderer);
    }
    spriteRenderer.Draw(bg);
    spriteRenderer.Render();
}
