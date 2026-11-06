#include "Game.h"

namespace
{
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 450;
}

Game::Game()
    : player_(385.0f, 310.0f),
      townObstacles_{
          { 240.0f, 65.0f, 125.0f, 95.0f }, { 435.0f, 65.0f, 125.0f, 95.0f },
          { 80.0f, 105.0f, 42.0f, 48.0f }, { 660.0f, 265.0f, 48.0f, 54.0f }
      },
      roomPlatforms_{
          { 0.0f, 390.0f, 800.0f, 60.0f },
          { 220.0f, 315.0f, 130.0f, 18.0f },
          { 455.0f, 255.0f, 135.0f, 18.0f }
      },
      tavernDoor_{ 365.0f, 145.0f, 70.0f, 65.0f },
      roomExit_{ 0.0f, 320.0f, 65.0f, 70.0f }
{
}

void Game::Update(float deltaTime)
{
    if (scene_ == Scene::Town) UpdateTown(deltaTime);
    else UpdateSideRoom(deltaTime);
}

void Game::UpdateTown(float deltaTime)
{
    player_.UpdateTopDown(deltaTime, townObstacles_);
    if (CheckCollisionRecs(player_.GetBounds(), tavernDoor_) && IsKeyPressed(KEY_E))
    {
        scene_ = Scene::SideRoom;
        player_.SetPosition(90.0f, 354.0f);
    }
}

void Game::UpdateSideRoom(float deltaTime)
{
    player_.UpdateSideScroller(deltaTime, roomPlatforms_);
    if (CheckCollisionRecs(player_.GetBounds(), roomExit_) && IsKeyPressed(KEY_E))
    {
        scene_ = Scene::Town;
        player_.SetPosition(385.0f, 220.0f);
    }
}

void Game::Draw() const
{
    BeginDrawing();
    if (scene_ == Scene::Town) DrawTown();
    else DrawSideRoom();
    EndDrawing();
}

void Game::DrawTown() const
{
    ClearBackground({ 94, 166, 83, 255 });
    DrawRectangle(0, 205, screenWidth, 95, { 202, 174, 117, 255 });
    DrawRectangle(355, 0, 90, screenHeight, { 202, 174, 117, 255 });

    DrawRectangle(225, 45, 345, 120, { 143, 78, 55, 255 });
    DrawRectangle(215, 35, 365, 22, { 95, 48, 42, 255 });
    DrawRectangle(365, 112, 70, 53, { 68, 42, 37, 255 });
    DrawRectangle(376, 122, 48, 43, { 191, 128, 68, 255 });
    DrawText("THE TAVERN", 320, 75, 22, RAYWHITE);

    DrawCircle(101, 101, 27, DARKGREEN);
    DrawRectangle(96, 112, 10, 35, BROWN);
    DrawCircle(684, 255, 31, DARKGREEN);
    DrawRectangle(679, 267, 10, 35, BROWN);

    DrawText("TOWN", 20, 18, 24, DARKGREEN);
    DrawText("WASD / arrows: move", 20, 388, 18, DARKGRAY);
    DrawText("Walk to the tavern door and press E", 20, 414, 18, DARKGRAY);
    player_.Draw();
}

void Game::DrawSideRoom() const
{
    ClearBackground({ 47, 42, 66, 255 });
    DrawRectangle(0, 0, screenWidth, 82, { 75, 59, 83, 255 });
    DrawText("TAVERN CELLAR", 24, 22, 26, RAYWHITE);
    DrawText("A side-view room for future dungeons and interiors", 24, 55, 17, LIGHTGRAY);

    for (std::size_t i = 0; i < roomPlatforms_.size(); ++i)
    {
        const Rectangle platform = roomPlatforms_[i];
        DrawRectangleRec(platform, i == 0 ? BROWN : Color{ 137, 101, 69, 255 });
        DrawRectangle(static_cast<int>(platform.x), static_cast<int>(platform.y),
                      static_cast<int>(platform.width), 6, { 89, 157, 82, 255 });
    }

    DrawRectangle(12, 329, 45, 61, { 102, 64, 48, 255 });
    DrawRectangleLines(12, 329, 45, 61, GOLD);
    DrawText("A / D or arrows: move", 20, 414, 18, RAYWHITE);
    DrawText("Space / W / Up: jump", 280, 414, 18, RAYWHITE);
    DrawText("Reach the left door and press E to return", 20, 388, 18, LIGHTGRAY);
    player_.Draw();
}
