#include "Bag.h"

void Bag::Init(int tetroPieceCount) {
    for (int i = 0; i < tetroPieceCount; i++) {
        bag1.Add(i);
        bag2.Add(i);
    }

    Shuffle(bag1);
    Shuffle(bag2);
}

int Bag::GetNextIndex() {
    Array<int>& bag = (!useSecondBag) ? bag1 : bag2;

    int next = bag[index++];

    if (index >= bag.Count()) {
        Shake();
        index = 0;
    }

    return next;
}

void Bag::Shake() {
    Array<int>& bag = (useSecondBag) ? bag1 : bag2;

    Shuffle(bag);
    useSecondBag = !useSecondBag;
}

void Bag::DrawNext(SpriteRenderer& spriteRenderer, Vec3 position, Slice<TetroPiece> pieces, Ref<SpriteAtlas> atlas, int count) {
    Array<int>& bag = (!useSecondBag) ? bag1 : bag2;
    
    for (int i = index; i < bag.Count(); i++) {
        pieces.Data()[bag[i]].Draw(spriteRenderer, position, Rotation::Degrees0, atlas, 1.0f);

        // @HARDCODED
        position.y -= TILE_SIZE.y * 2.5f;

        if (i - index >= count) return;
    }

    Array<int>& otherBag = (useSecondBag) ? bag1 : bag2;
    int nextBagCount = count - (count - index);
    for (int i = 0; i < nextBagCount; ++i) {
        pieces.Data()[otherBag.Data()[i]].Draw(spriteRenderer, position, Rotation::Degrees0, atlas, 1.0f);

        // @HARDCODED
        position.y -= TILE_SIZE.y * 2.5f;
    }
}

void Bag::Shuffle(Array<int>& bag) {
    for (int i = 0; i < bag.Count() - 1; ++i) {
        int j = i + (int)(RandomRange() / (RANDOM_MAX / ((u64)bag.Count() - i) + 1));
        int t = bag[j];
        bag[j] = bag[i];
        bag[i] = t;
    }
}
