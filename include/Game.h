#pragma once

#include "Player.h"
#include "Creature.h"
#include <vector>

class Game
{
public:
    Game();
    void Update(float deltaTime);
    void Draw() const;

private:
    struct Arrow
    {
        Vector2 position;
        float direction;
        float distanceTraveled = 0.0f;
        float verticalVelocity = 0.0f;
    };

    enum class Scene { Town, SideRoom };
    void UpdateTown(float deltaTime);
    void UpdateSideRoom(float deltaTime);
    void DrawTown() const;
    void DrawSideRoom() const;
    void DrawHealth(int x, int y) const;

    Scene scene_ = Scene::Town;
    Player player_;
    Creature creature_;
    std::vector<Arrow> arrows_;
    std::vector<Rectangle> townObstacles_;
    std::vector<Rectangle> roomPlatforms_;
    Rectangle tavernDoor_;
    Rectangle roomExit_;
};
