#pragma once

#include "Pandora/Scene.h"

#include "Board.h"
#include "Bag.h"

using namespace pd;

class GameScene : public Scene {
public:
    GameScene(App* app, StringView name, JsonValue* config);

    virtual void OnUnfocus() override;

    virtual void OnUpdate(f32 dt) override;
    virtual void OnRender(f32 dt) override;

private:
    void ResetBoard();
    void NewGhost(TetroPiece* preferred = nullptr);

    Sprite bg;

    Board board;
    Bag bag;
    SpriteRenderer spriteRenderer;
    Array<TetroPiece> pieces;
    Ref<SpriteAtlas> tileAtlas;

    f32 fallTimer = 0.0f;
    f32 fallRateInSeconds = startingFallRate;

    f32 startingFallRate = 1.0f;
    f32 finalFallRate = 0.2f;

    int maxScore = 5000;

    // Cosmetic subposition of the Y for smooth falling
    bool tweenY = true;
    f32 subY = 0.0f;

    f32 lockDelayTimer = 0.0f;
    f32 lockDelayInSeconds = 0.5f;
    bool lockToGround = false;

    // If sonic lock is enabled it means that there will be no lock delay on a hard drop.
    bool sonicLock = true;

    Sound dropSfx;
    Sound rotateSfx;
    Sound holdSfx;

    Text heldText;
    Text nextText;
    Text scoreText;

    TetroPiece* ghostPiece = nullptr;
    Rotation ghostRotation = Rotation::Degrees0;
    Vec2i ghostPos = Vec2i(BOARD_WIDTH / 2 - 2, BOARD_VISIBLE_HEIGHT);

    TetroPiece* heldPiece = nullptr;
    bool alreadyHeld = false;
};
