#pragma once

#include "TetroPiece.h"

class Bag {
public:
    void Init(int tetroPieceCount);
    int GetNextIndex();

    void Shake();

    void DrawNext(SpriteRenderer& spriteRenderer, Vec3 position, Slice<TetroPiece> pieces, Ref<SpriteAtlas> atlas, int count);

private:
    void Shuffle(Array<int>& bag);

    Array<int> bag1;
    Array<int> bag2;
    int index = 0;
    bool useSecondBag = false;
};
