#pragma once

#include "Pandora/Core.h"
#include "Pandora/Graphics.h"

using namespace pd;

const Vec2 TILE_SIZE = Vec2(32.0f);

enum class TileType : byte {
    Empty,
    Cyan,   // I
    Yellow, // O
    Purple, // T
    Green,  // S
    Red,    // Z
    Blue,   // J
    Orange, // L
    Count
};

const StringView TILE_TYPE_NAMES[] = {
    "Empty",
    "Cyan",
    "Yellow",
    "Purple",
    "Green",
    "Red",
    "Blue",
    "Orange"
};

// Count minus the empty tile
const int TETROPIECE_COUNT = (int)TileType::Count - 1;

enum class Rotation {
    Degrees0,
    Degrees270,
    Degrees180,
    Degrees90,
    Count
};

int GetRotatedIndex(int x, int y, Rotation rotation);

class TetroPiece {
public:
    void SetShape(StringView shape);
    void SetType(TileType type);

    void SetRotationOffset(Vec2i degrees0, Vec2i degrees90, Vec2i degrees180, Vec2i degrees270);

    TileType GetType() const;
    void GetShape(bool shape[4*4]);
    Vec2i GetRotationOffset(Rotation rotation);

    void GetRotatedShape(bool shape[4*4], Rotation rotation);

    void Draw(SpriteRenderer& spriteRenderer, Vec3 position, Rotation rotation, Ref<SpriteAtlas> atlas, f32 alpha);

private:
    TileType type = TileType::Empty;
    bool shape[4*4] = {};
    Vec2i rotationOffset[(int)Rotation::Count];
};
