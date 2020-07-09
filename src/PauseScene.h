#pragma once

#include "Pandora/Core.h"
#include "Pandora/Graphics.h"
#include "Pandora/Audio.h"
#include "Pandora/Scene.h"

using namespace pd;

class PauseScene : public Scene {
public:
    PauseScene(App* app, StringView name);

    virtual void OnFocus() override;

    virtual void OnUpdate(f32 dt) override;
    virtual void OnRender(f32 dt) override;

private:
    SpriteRenderer spriteRenderer;
    Vec2 optionOffset;

    Array<Text> options;
    int index = 0;

    Sound selectSfx;
    Sound confirmSfx;

    Sprite bg;
};
