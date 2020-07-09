#pragma once

#include <Pandora/Audio.h>

#include "TetroPiece.h"

// @HARDCODED
const int BOARD_WIDTH = 10;
const int BOARD_HEIGHT = 40;
const int BOARD_VISIBLE_HEIGHT = 21;

class Board {
public:
    Board(StringView tileAtlasName);

    bool DoesPieceFit(Vec2i position, TetroPiece* piece, Rotation rotation);
    void AddPieceToBoard(Vec2i position, TetroPiece* piece, Rotation rotation);

    void Draw(SpriteRenderer& spriteRenderer);

    void Clear();

    TileType GetTileAtPosition(int x, int y);

    void StopSFX();

    void SetClearScores(int scores[4]);
    void SetSFXVolume(f32 volume);

    int GetScore();

    Ref<SpriteAtlas> GetTileAtlas(RefType type = RefType::Strong);

    Vec3 position;
    Vec2 scale = Vec2(1.0f);

private:
    // Playfield includes the so-called "vanish zone"
    TileType board[BOARD_HEIGHT][BOARD_WIDTH] = {};

    Ref<SpriteAtlas> tileAtlas;

    Sound clearSfx;
    Sound superClearSfx;

    int clearScores[4] = {
        40,
        100,
        300,
        800
    };

    // @TODO: implement level mechanic
    int level = 0;
    int score = 0;
};
