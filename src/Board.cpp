#include "Board.h"

Board::Board(StringView tileAtlasName) {
    tileAtlas = Ref<SpriteAtlas>::Create();
    tileAtlas->Load(Vec2(0.0f), TILE_SIZE, tileAtlasName);

    clearSfx.Load("SFX/Clear");
    superClearSfx.Load("SFX/SuperClear");

    MemorySet(board, BOARD_WIDTH * BOARD_HEIGHT, (byte)TileType::Empty);
}

bool Board::DoesPieceFit(Vec2i position, TetroPiece* piece, Rotation rotation) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            int boardX = position.x + x;
            int boardY = position.y + y;

            bool shape[4 * 4];
            piece->GetRotatedShape(shape, rotation);

            if (boardX >= 0 && boardX < BOARD_WIDTH &&
                boardY >= 0 && boardY < BOARD_HEIGHT) {
                // Check if there is a tetrimino there
                if (shape[y * 4 + x] && board[boardY][boardX] != TileType::Empty) {
                    return false;
                }
            } else if (shape[y * 4 + x]) {
                // Out of bounds
                return false;
            }
        }
    }

    return true;
}

void Board::AddPieceToBoard(Vec2i position, TetroPiece* piece, Rotation rotation) {
    if (!DoesPieceFit(position, piece, rotation)) return;

    // Add it to the board
    bool shape[4 * 4];
    piece->GetRotatedShape(shape, rotation);

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (shape[y * 4 + x]) {
                int finalX = position.x + x;
                int finalY = position.y + y;

                board[finalY][finalX] = piece->GetType();
            }
        }
    }

    // Check for cleared lines
    Array<int> clearedLines(Allocator::Temporary);
    for (int y = 0; y < 4; ++y) {
        if (position.y + y < BOARD_HEIGHT) {
            bool line = true;
            for (int x = 0; x < BOARD_WIDTH; ++x) {
                line &= board[position.y + y][x] != TileType::Empty;
            }

            if (line) {
                clearedLines.Add(position.y + y);
            }
        }
    }

    // If we've cleared any lines, move all the tiles from top to bottom
    if (clearedLines.Count() > 0) {
        for (int i = clearedLines.Count() - 1; i >= 0; i--) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                board[clearedLines[i]][x] = TileType::Empty;

                for (int y = clearedLines[i] + 1; y < BOARD_HEIGHT; ++y) {
                    board[y - 1][x] = board[y][x];
                }
            }
        }
    }

    // Play the appropiate sound effect
    if (clearedLines.Count() >= 4) {
        superClearSfx.Play();
    } else if (clearedLines.Count() >= 1) {
        clearSfx.Play();
    }

    // Add score
    // @HARDCODED
    switch (clearedLines.Count()) {
        case 1:
            score += clearScores[0] * (level + 1);
            break;

        case 2:
            score += clearScores[1] * (level + 1);
            break;

        case 3:
            score += clearScores[2] * (level + 1);
            break;

        case 4:
            score += clearScores[3] * (level + 1);
            break;
    }
}

void Board::Draw(SpriteRenderer& spriteRenderer) {
    Vec3 drawPos = position;

    for (int y = 0; y < BOARD_VISIBLE_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            Sprite* tile = tileAtlas->Get((int)board[y][x], 0);
            tile->position = drawPos;
            tile->size = tile->GetSize(0, false);
            tile->scale = TILE_SIZE / tile->size;
            tile->color.a = 1.0f;
            spriteRenderer.Draw(*tile);

            drawPos.x += TILE_SIZE.x * scale.x;
        }

        drawPos.y += TILE_SIZE.y * scale.y;
        drawPos.x = position.x;
    }
}

void Board::Clear() {
    score = 0;
    MemorySet(board, BOARD_WIDTH * BOARD_HEIGHT, (byte)TileType::Empty);
}

TileType Board::GetTileAtPosition(int x, int y) {
    return board[y][x];
}

void Board::StopSFX() {
    clearSfx.Stop();
    superClearSfx.Stop();
}

void Board::SetClearScores(int scores[4]) {
    MemoryCopy(clearScores, scores, sizeof(int) * 4);
}

void Board::SetSFXVolume(f32 volume) {
    clearSfx.volume = volume;
    superClearSfx.volume = volume;
}

int Board::GetScore() {
    return score;
}

Ref<SpriteAtlas> Board::GetTileAtlas(RefType type) {
    return tileAtlas.NewRef(type);
}
