#pragma once

#include "Pandora/Core.h"
#include "Pandora/Graphics.h"
#include "Pandora/Audio.h"
#include "Pandora/Scene.h"

using namespace pd;

class MenuScene : public Scene {
public:
    MenuScene(App* app, StringView name, JsonValue* config);

    virtual void OnFocus() override;
    virtual void OnUnfocus() override;

    virtual void OnUpdate(f32 dt) override;
    virtual void OnRender(f32 dt) override;

private:
    SpriteRenderer spriteRenderer;
    Vec2 optionOffset;

    Array<Text> options;
    int index = 0;

    Sound bgm;
    Sound selectSfx;
    Sound confirmSfx;

    Sprite bg;
    JsonValue* config = nullptr;
};
