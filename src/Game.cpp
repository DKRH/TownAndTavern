#include "Game.h"
#include <cmath>
#include <cstddef>

namespace
{
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 450;
    constexpr float arrowSpeed = 560.0f;
    constexpr float arrowDropStartDistance = 300.0f;
    constexpr float arrowGravity = 420.0f;
}

Game::Game()
    : player_(385.0f, 310.0f),
      creature_(640.0f, 352.0f),
      roomPlatforms_{
          { 0.0f, 390.0f, 800.0f, 60.0f },
          { 220.0f, 315.0f, 130.0f, 18.0f },
          { 455.0f, 255.0f, 135.0f, 18.0f }
      },
      roomExit_{ 0.0f, 320.0f, 65.0f, 70.0f }
{
}

void Game::Update(float deltaTime)
{
    if (scene_ == Scene::Town &&
        (IsKeyPressed(KEY_F1) || (editingTown_ && IsKeyPressed(KEY_ESCAPE))))
    {
        editingTown_ = !editingTown_;
        return;
    }
    if (scene_ == Scene::Town && editingTown_)
    {
        townLevel_.UpdateEditor();
        return;
    }

    player_.UpdateCombat(deltaTime);
    if (scene_ == Scene::Town) UpdateTown(deltaTime);
    else UpdateSideRoom(deltaTime);
}

void Game::UpdateTown(float deltaTime)
{
    player_.UpdateTopDown(deltaTime, townLevel_.GetTownObstacles());
    Rectangle tavernDoor{};
    if (townLevel_.GetTavernDoor(tavernDoor) &&
        CheckCollisionRecs(player_.GetBounds(), tavernDoor) && IsKeyPressed(KEY_E))
    {
        scene_ = Scene::SideRoom;
        player_.SetPosition(90.0f, 354.0f);
    }
}

void Game::UpdateSideRoom(float deltaTime)
{
    player_.UpdateSideScroller(deltaTime, roomPlatforms_);

    if (player_.ConsumeAttackRequest())
    {
        if (player_.GetWeapon() == Player::Weapon::Sword)
        {
            const float playerCenterX = player_.GetBounds().x + player_.GetBounds().width / 2.0f;
            const float creatureCenterX = creature_.GetBounds().x + creature_.GetBounds().width / 2.0f;
            const float distanceX = creatureCenterX - playerCenterX;
            const float distanceY = std::abs((creature_.GetBounds().y + creature_.GetBounds().height) -
                                             (player_.GetBounds().y + player_.GetBounds().height));
            if (creature_.IsAlive() && std::abs(distanceX) <= 57.0f && distanceY < 34.0f &&
                distanceX * player_.GetFacing() > 0.0f)
            {
                creature_.TakeDamage(1);
                creature_.KnockBackFrom(playerCenterX, 14.0f);
            }
        }
        else
        {
            const Rectangle playerBounds = player_.GetBounds();
            arrows_.push_back({ { playerBounds.x + playerBounds.width / 2.0f,
                                  playerBounds.y + playerBounds.height / 2.0f },
                                player_.GetFacing() });
        }
    }

    const bool creatureAttacked = creature_.Update(deltaTime, player_.GetBounds());
    if (creatureAttacked && CheckCollisionRecs(creature_.GetBounds(), player_.GetBounds()))
    {
        const float creatureCenterX = creature_.GetBounds().x + creature_.GetBounds().width / 2.0f;
        if (player_.IsDefending())
        {
            player_.KnockBackFrom(creatureCenterX, 14.0f);
        }
        else if (!player_.IsInvulnerable())
        {
            player_.TakeDamage(1);
            player_.KnockBackFrom(creatureCenterX, 28.0f);
        }
    }

    for (std::size_t i = 0; i < arrows_.size();)
    {
        Arrow& arrow = arrows_[i];
        const float horizontalStep = arrow.direction * arrowSpeed * deltaTime;
        arrow.position.x += horizontalStep;
        arrow.distanceTraveled += std::abs(horizontalStep);
        if (arrow.distanceTraveled > arrowDropStartDistance)
        {
            arrow.verticalVelocity += arrowGravity * deltaTime;
            arrow.position.y += arrow.verticalVelocity * deltaTime;
        }
        const Rectangle arrowBounds = { arrow.position.x - 8.0f, arrow.position.y - 2.0f, 16.0f, 4.0f };
        if (creature_.IsAlive() && CheckCollisionRecs(arrowBounds, creature_.GetBounds()))
        {
            creature_.TakeDamage(1);
            creature_.KnockBackFrom(arrow.position.x - arrow.direction * 4.0f, 8.0f);
            arrows_.erase(arrows_.begin() + static_cast<std::ptrdiff_t>(i));
        }
        else if (arrow.position.x < -20.0f || arrow.position.x > 820.0f || arrow.position.y > screenHeight)
        {
            arrows_.erase(arrows_.begin() + static_cast<std::ptrdiff_t>(i));
        }
        else ++i;
    }

    if (player_.GetHealth() <= 0)
    {
        scene_ = Scene::Town;
        player_.RestoreHealth();
        player_.SetPosition(385.0f, 220.0f);
        arrows_.clear();
    }

    if (CheckCollisionRecs(player_.GetBounds(), roomExit_) && IsKeyPressed(KEY_E))
    {
        scene_ = Scene::Town;
        player_.SetPosition(385.0f, 220.0f);
    }
}

void Game::Draw() const
{
    BeginDrawing();
    if (scene_ == Scene::Town && editingTown_) townLevel_.DrawEditor();
    else if (scene_ == Scene::Town) DrawTown();
    else DrawSideRoom();
    EndDrawing();
}

void Game::DrawTown() const
{
    ClearBackground(BLACK);
    townLevel_.DrawTown();
    DrawRectangle(0, 384, screenWidth, 66, { 219, 218, 203, 255 });
    DrawText("TOWN  |  WASD / arrows: move", 20, 390, 18, DARKGREEN);
    DrawText("Walk to the tavern door and press E  |  F1: level editor", 20, 416, 18, DARKGRAY);
    DrawHealth(650, 20);
    DrawText(TextFormat("Weapon: %s (Q)", player_.GetWeapon() == Player::Weapon::Sword ? "Sword" : "Bow"),
             570, 48, 17, DARKGREEN);
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
    if (creature_.IsAlive()) creature_.Draw();
    else DrawText("The cellar is clear!", 600, 110, 18, LIME);
    for (const Arrow& arrow : arrows_)
    {
        DrawLineEx({ arrow.position.x - arrow.direction * 8.0f, arrow.position.y },
                   { arrow.position.x + arrow.direction * 8.0f,
                     arrow.position.y + arrow.verticalVelocity * 0.025f }, 3.0f, GOLD);
    }
    DrawHealth(630, 22);
    DrawText(TextFormat("Weapon: %s (Q)", player_.GetWeapon() == Player::Weapon::Sword ? "Sword" : "Bow"),
             610, 47, 17, RAYWHITE);
    if (player_.GetWeapon() == Player::Weapon::Sword)
        DrawText("J / click: swing   K / right-click: block", 20, 360, 17, LIGHTGRAY);
    else
        DrawText("J / click: shoot   Shift: evade backwards", 20, 360, 17, LIGHTGRAY);
    player_.Draw();
}

void Game::DrawHealth(int x, int y) const
{
    DrawText("HP", x, y, 18, RAYWHITE);
    for (int i = 0; i < player_.GetMaxHealth(); ++i)
        DrawRectangle(x + 34 + i * 20, y + 2, 16, 14,
                      i < player_.GetHealth() ? RED : DARKGRAY);
}
