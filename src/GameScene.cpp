#include "GameScene.h"

Rotation NextRotation(Rotation current, bool clockwise = true) {
    if (clockwise) {
        return (Rotation)((int(current) + 1) % (int)Rotation::Count);
    } else {
        int next = (int)current - 1;
        if (next < 0) {
            next = (int)Rotation::Count - 1;
        }
        return Rotation(next);
    }
}

GameScene::GameScene(App* app, StringView name, JsonValue* config) : Scene(app, name), board("Tiles") {
    // Nudge camera
    Vec2i size = window.GetSize();

    cam.Init(size);
    cam.position.xy += (Vec2)size / 2.0f;

    board.position = Vec3(TILE_SIZE.x, TILE_SIZE.y, 100.0f);
    tileAtlas = board.GetTileAtlas();
    bg.Load("GameBG");

    // Load config
    config->TryGetBool("smooth_fall", tweenY);
    config->TryGetBool("sonic_lock", sonicLock);

    JsonValue scores;
    if (config->TryGetArray("scores", scores)) {
        // @HARDCODED: default scores
        int clearScores[4] = {
            40,
            100,
            300,
            800
        };

        f64 tmp = 0.0;
        if (scores.TryGetNumber(0, tmp)) {
            clearScores[0] = (int)tmp;
        }
        if (scores.TryGetNumber(1, tmp)) {
            clearScores[1] = (int)tmp;
        }
        if (scores.TryGetNumber(2, tmp)) {
            clearScores[2] = (int)tmp;
        }
        if (scores.TryGetNumber(3, tmp)) {
            clearScores[3] = (int)tmp;
        }

        board.SetClearScores(clearScores);
    }

    f64 startingFallSpeed = 0.0;
    f64 finalFallSpeed = 0.0;

    if (config->TryGetNumber("starting_fall_speed", startingFallSpeed)) {
        startingFallRate = (f32)startingFallSpeed;
        fallRateInSeconds = startingFallRate;
    }

    if (config->TryGetNumber("final_fall_speed", finalFallSpeed)) {
        finalFallRate = (f32)finalFallSpeed;
    }

    f64 maxScore = 0.0;
    if (config->TryGetNumber("max_score", maxScore)) {
        this->maxScore = (int)maxScore;
    }

    f64 sfxVolume;
    if (config->TryGetNumber("sfx_volume", sfxVolume)) {
        dropSfx.volume = (f32)sfxVolume;
        rotateSfx.volume = (f32)sfxVolume;
        holdSfx.volume = (f32)sfxVolume;

        board.SetSFXVolume((f32)sfxVolume);
    }

    JsonValue piecesConfig;
    if (config->TryGetArray("pieces", piecesConfig)) {
        int count = piecesConfig.Count();
        for (int i = 0; i < count; i++) {
            JsonValue pieceConfig;
            if (piecesConfig.TryGetObject(i, pieceConfig)) {
                String shape;
                shape.Set("....\nX.X.\n.X..\nX.X.");
                pieceConfig.TryGetString("shape", shape);

                String colorStr;
                TileType color = TileType::Cyan;
                if (pieceConfig.TryGetString("color", colorStr)) {
                    // Start at 1 to exclude TileType::Empty
                    for (int i = 1; i < (int)TileType::Count; i++) {
                        if (colorStr == TILE_TYPE_NAMES[i]) {
                            color = (TileType)i;
                            break;
                        }
                    }
                }

                Vec2i rotations[4] = {
                    Vec2i(0, -1),
                    Vec2i(1, 0),
                    Vec2i(0, 1),
                    Vec2i(-1, 0)
                };

                JsonValue offsetConfig;
                if (pieceConfig.TryGetArray("offset", offsetConfig)) {
                    for (int i = 0; i < 4; i++) {
                        JsonValue offsetArray;
                        offsetConfig.TryGetArray(i, offsetArray);

                        f64 x, y;
                        if (offsetArray.TryGetNumber(0, x)) {
                            rotations[i].x = (int)x;
                        }
                        if (offsetArray.TryGetNumber(1, y)) {
                            rotations[i].y = (int)y;
                        }
                    }
                }

                pieces.Add();
                pieces.Last().SetType(color);
                pieces.Last().SetShape(shape);
                pieces.Last().SetRotationOffset(rotations[0], rotations[1], rotations[2], rotations[3]);
            }
        }
    } else {
        // Default pieces
        pieces.Add();
        pieces.Last().SetType(TileType::Cyan);
        pieces.Last().SetShape(R"(.... 
                              ....
                              XXXX
                              ....)");
        pieces.Last().SetRotationOffset(Vec2i(0, -1), Vec2i(1, 0), Vec2i(0, 1), Vec2i(-1, 0));

        pieces.Add();
        pieces.Last().SetType(TileType::Red);
        pieces.Last().SetShape("....\n....\n.XX.\nXX..");
        pieces.Last().SetRotationOffset(Vec2i(0, -1), Vec2i(1, 0), Vec2i(0, 1), Vec2i(-1, 0));

        pieces.Add();
        pieces.Last().SetType(TileType::Green);
        pieces.Last().SetShape("....\n....\nXX..\n.XX.");
        pieces.Last().SetRotationOffset(Vec2i(0, -1), Vec2i(1, 0), Vec2i(0, 1), Vec2i(-1, 0));

        pieces.Add();
        pieces.Last().SetType(TileType::Yellow);
        pieces.Last().SetShape("....\n....\nXX..\nXX..");
        pieces.Last().SetRotationOffset(Vec2i(0, -2), Vec2i(2, 0), Vec2i(0, 2), Vec2i(-2, 0));

        pieces.Add();
        pieces.Last().SetType(TileType::Purple);
        pieces.Last().SetShape("....\n....\nXXX.\n.X..");
        pieces.Last().SetRotationOffset(Vec2i(0, -1), Vec2i(1, 0), Vec2i(0, 1), Vec2i(-1, 0));

        pieces.Add();
        pieces.Last().SetShape("....\n....\nXXX.\n..X.");
        pieces.Last().SetType(TileType::Orange);
        pieces.Last().SetRotationOffset(Vec2i(0, -1), Vec2i(1, 0), Vec2i(0, 1), Vec2i(-1, 0));

        pieces.Add();
        pieces.Last().SetShape("....\n....\nXXX.\nX...");
        pieces.Last().SetType(TileType::Blue);
        pieces.Last().SetRotationOffset(Vec2i(0, -1), Vec2i(1, 0), Vec2i(0, 1), Vec2i(-1, 0));
    }

    bag.Init(pieces.Count());
    bag.Shake();

    NewGhost();

    // Initialize HUD
    heldText.Load("GuiFont");
    heldText.text.Set("Held:");
    heldText.scale = Vec2(0.33f);
    heldText.position = Vec3(BOARD_WIDTH + 2.0f, BOARD_VISIBLE_HEIGHT - 1, 100.0f) * Vec3(TILE_SIZE.x, TILE_SIZE.y, 200.0f);
    heldText.GenerateSprites();

    nextText.Load("GuiFont");
    nextText.text.Set("Next:");
    nextText.scale = Vec2(0.33f);
    nextText.position = Vec3(BOARD_WIDTH + 2.0f, BOARD_VISIBLE_HEIGHT - 6, 100.0f) * Vec3(TILE_SIZE.x, TILE_SIZE.y, 200.0f);
    nextText.GenerateSprites();

    scoreText.Load("GuiFont");
    scoreText.text.Set("Score: 0");
    scoreText.scale = Vec2(0.33f);
    scoreText.position = Vec3(4.0f, -TILE_SIZE.y / 2.0f, 200.0f);
    scoreText.GenerateSprites();

    dropSfx.Load("SFX/Drop");
    rotateSfx.Load("SFX/Rotate");
    holdSfx.Load("SFX/Hold");
}

void GameScene::OnUnfocus() {
    dropSfx.Stop();
    rotateSfx.Stop();
    holdSfx.Stop();

    board.StopSFX();
}

void GameScene::OnUpdate(f32 dt) {
    fallTimer += dt;

    // Lock to the ground
    bool canGoDown = board.DoesPieceFit(ghostPos - Vec2i(0, 1), ghostPiece, ghostRotation);

    subY = (canGoDown && tweenY) ? fallTimer / fallRateInSeconds : 0.0f;
    if (fallTimer >= fallRateInSeconds && canGoDown) {
        if (canGoDown) {
            fallTimer = 0.0f;
            ghostPos.y -= 1;
            lockDelayTimer = 0.0f;
            lockToGround = false;
            subY = 0.0f;

            // Recalculate
            canGoDown = board.DoesPieceFit(ghostPos - Vec2i(0, 1), ghostPiece, ghostRotation);
        }
    }

    if (!canGoDown) {
        lockToGround = true;
    }

    // Check if we can lock
    if (lockToGround) {
        if (lockDelayTimer >= lockDelayInSeconds) {
            lockToGround = false;
            lockDelayTimer = 0.0f;
            alreadyHeld = false;

            dropSfx.Play();

            board.AddPieceToBoard(ghostPos, ghostPiece, ghostRotation);
            NewGhost();

            // Update score
            scoreText.text.Format("Score: {}", board.GetScore());
            scoreText.GenerateSprites();

            // Update speed
            fallRateInSeconds = Lerp(startingFallRate, finalFallRate, board.GetScore() / (f32)maxScore);

        } else {
            lockDelayTimer += dt;
        }
    }

    // Rotate
    if (input.IsKeyPressed(Key::Z) || input.IsKeyPressed(Key::X)) {
        bool rotationSuccess = true;
        Vec2i wallKickOffset = Vec2i(0);

        bool clockwise = input.IsKeyPressed(Key::Z);

        Rotation rotationForOffset = (clockwise) ? NextRotation(ghostRotation) : ghostRotation;
        Vec2i baseOffset = ghostPiece->GetRotationOffset(rotationForOffset);

        if (!clockwise) {
            baseOffset = -baseOffset;
        }

        Vec2i offsetPos = ghostPos + baseOffset;

        Rotation nextRotation = NextRotation(ghostRotation, clockwise);
        if (!board.DoesPieceFit(offsetPos, ghostPiece, nextRotation)) {
            // Tetromino doesn't fit, we must wall kick
            // Will check from offsetPos.x -..+ WALL_KICK_RANGE
            const int WALL_KICK_RANGE = 2;

            rotationSuccess = false;
            for (int x = 1; x <= WALL_KICK_RANGE; x++) {
                if (board.DoesPieceFit(offsetPos + Vec2i(x, 0), ghostPiece, nextRotation)) {
                    rotationSuccess = true;
                    wallKickOffset.x = x;
                    break;
                }
            }

            if (!rotationSuccess) {
                for (int x = -1; x >= -WALL_KICK_RANGE; x--) {
                    if (board.DoesPieceFit(offsetPos + Vec2i(x, 0), ghostPiece, nextRotation)) {
                        rotationSuccess = true;
                        wallKickOffset.x = x;
                        break;
                    }
                }
            }
        }

        if (rotationSuccess) {
            rotateSfx.Play();

            ghostRotation = nextRotation;
            ghostPos = offsetPos + wallKickOffset;

            // We reset the lock delay on rotation in this household
            lockDelayTimer = 0.0f;
        }
    }

    // Hold
    if (!alreadyHeld && input.IsKeyPressed(Key::C)) {
        holdSfx.Play();

        TetroPiece* currentHeld = heldPiece;
        heldPiece = ghostPiece;
        NewGhost(currentHeld);
        alreadyHeld = true;
    }

    // Move left
    if (input.IsKeyRepeated(Key::Left) &&
        board.DoesPieceFit(ghostPos - Vec2i(1, 0), ghostPiece, ghostRotation)) {
        ghostPos.x -= 1;
        lockDelayTimer = 0.0f;
    }

    // Move right
    if (input.IsKeyRepeated(Key::Right) &&
        board.DoesPieceFit(ghostPos + Vec2i(1, 0), ghostPiece, ghostRotation)) {
        ghostPos.x += 1;
        lockDelayTimer = 0.0f;
    }

    // Move down
    if (input.IsKeyRepeated(Key::Down) &&
        board.DoesPieceFit(ghostPos - Vec2i(0, 1), ghostPiece, ghostRotation)) {
        ghostPos.y -= 1;
    }

    // Hard drop
    if (input.IsKeyPressed(Key::Space)) {
        int y = 0;
        while (board.DoesPieceFit(ghostPos - Vec2i(0, ++y), ghostPiece, ghostRotation));
        ghostPos.y -= y - 1;

        if (sonicLock) {
            lockToGround = true;
            lockDelayTimer = lockDelayInSeconds;
        }
    }

    // Reset
    if (input.IsKeyPressed(Key::R)) {
        ResetBoard();
    }

    cam.UpdateProjection();
}

void GameScene::OnRender(f32 dt) {
    spriteRenderer.UpdateProjection(cam.GetMatrix());
    board.Draw(spriteRenderer);
    spriteRenderer.Draw(bg);

    Vec3 worldPos = board.position + Vec3(ghostPos.x * TILE_SIZE.x, ghostPos.y * TILE_SIZE.y, 1.0f);
    ghostPiece->Draw(spriteRenderer, worldPos - Vec3(0.0f, subY * TILE_SIZE.y, 0.0f), ghostRotation, tileAtlas, 1.0f);

    // Render preview
    int lowestY = 1;
    while (board.DoesPieceFit(ghostPos - Vec2i(0, lowestY), ghostPiece, ghostRotation)) {
        ++lowestY;
    }
    --lowestY;

    ghostPiece->Draw(spriteRenderer, worldPos - Vec3(0.0f, lowestY * TILE_SIZE.y, 0.0f), ghostRotation, tileAtlas, 0.33f);

    // Render held
    Vec3 boardEnd = board.position + Vec3(BOARD_WIDTH * TILE_SIZE.x, BOARD_VISIBLE_HEIGHT * TILE_SIZE.y, 0.0f);
    if (heldPiece) {
        heldPiece->Draw(spriteRenderer, boardEnd + Vec3(TILE_SIZE.x * 2.0f, -TILE_SIZE.y * 6.0f, 0.0f), Rotation::Degrees0, tileAtlas, 1.0f);
    }

    // Render next
    bag.DrawNext(spriteRenderer, boardEnd + Vec3(TILE_SIZE.x * 2.0f, -TILE_SIZE.y * 11.0f, 100.0f), pieces, tileAtlas, 5);

    // Render HUD
    heldText.Draw(spriteRenderer);
    nextText.Draw(spriteRenderer);
    scoreText.Draw(spriteRenderer);

    spriteRenderer.Render();
}

void GameScene::ResetBoard() {
    fallRateInSeconds = startingFallRate;
    fallTimer = 0.0f;
    lockDelayTimer = 0.0f;

    board.Clear();

    bag.Shake();
    heldPiece = nullptr;
    NewGhost();

    scoreText.text.Set("Score: 0");
    scoreText.GenerateSprites();
}

void GameScene::NewGhost(TetroPiece* preferred) {
    // Generate new tetromino
    TetroPiece* newPiece = (preferred) ? preferred : &pieces[bag.GetNextIndex()];

    ghostRotation = Rotation::Degrees0;
    ghostPiece = newPiece;
    ghostPos = Vec2i(BOARD_WIDTH / 2 - 2, BOARD_VISIBLE_HEIGHT - 1);

    if (!board.DoesPieceFit(ghostPos, ghostPiece, ghostRotation)) {
        // GAME OVER
        ResetBoard();
    }
}
