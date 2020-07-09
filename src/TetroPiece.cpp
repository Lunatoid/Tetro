#include "TetroPiece.h"

int GetRotatedIndex(int x, int y, Rotation rotation) {
    switch (rotation) {
        case Rotation::Degrees0:
            return y * 4 + x;

        case Rotation::Degrees90:
            return 12 + y - (x * 4);

        case Rotation::Degrees180:
            return 15 - (y * 4) - x;

        case Rotation::Degrees270:
            return 3 - y + (x * 4);
    }

    return 0;
}

void TetroPiece::SetShape(StringView shape) {
    int i = 0;
    for (int s = 0; s < shape.Count(); s++) {
        if (shape[s] != 'X' && shape[s] != '.') continue;

        this->shape[i++] = shape[s] == 'X';

        if (i >= 4 * 4) break;
    }
}

void TetroPiece::SetType(TileType type) {
    this->type = type;
}

void TetroPiece::SetRotationOffset(Vec2i degrees0, Vec2i degrees90, Vec2i degrees180, Vec2i degrees270) {
    rotationOffset[(int)Rotation::Degrees0] = degrees0;
    rotationOffset[(int)Rotation::Degrees90] = degrees90;
    rotationOffset[(int)Rotation::Degrees180] = degrees180;
    rotationOffset[(int)Rotation::Degrees270] = degrees270;
}

TileType TetroPiece::GetType() const {
    return type;
}

void TetroPiece::GetShape(bool shape[4 * 4]) {
    MemoryCopy(shape, this->shape, 4 * 4);
}

Vec2i TetroPiece::GetRotationOffset(Rotation rotation) {
    return rotationOffset[(int)rotation];
}

void TetroPiece::GetRotatedShape(bool shape[4 * 4], Rotation rotation) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            shape[y * 4 + x] = this->shape[GetRotatedIndex(x, y, rotation)];
        }
    }
}

void TetroPiece::Draw(SpriteRenderer& spriteRenderer, Vec3 position, Rotation rotation, Ref<SpriteAtlas> atlas, f32 alpha) {
    bool rotated[4 * 4] = {};
    GetRotatedShape(rotated, rotation);

    Sprite* curr = atlas->Get((int)type, 0);
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            curr->position = position + Vec3(x * TILE_SIZE.x, y * TILE_SIZE.y, 0.0f);
            curr->size = curr->GetSize(0, true);
            curr->scale = TILE_SIZE / curr->size;

            curr->color.a = (rotated[y * 4 + x]) ? alpha : 0.0f;

            spriteRenderer.Draw(*curr);
        }
    }
}
