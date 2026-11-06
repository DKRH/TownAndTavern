#pragma once

#include "Player.h"
#include "Creature.h"
#include "TopDownLevel.h"
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
    bool editingTown_ = false;
    Player player_;
    Creature creature_;
    TopDownLevel townLevel_;
    std::vector<Arrow> arrows_;
    std::vector<Rectangle> roomPlatforms_;
    Rectangle roomExit_;
};
