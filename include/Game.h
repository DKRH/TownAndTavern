#pragma once

#include "Player.h"
#include <vector>

class Game
{
public:
    Game();
    void Update(float deltaTime);
    void Draw() const;

private:
    enum class Scene { Town, SideRoom };
    void UpdateTown(float deltaTime);
    void UpdateSideRoom(float deltaTime);
    void DrawTown() const;
    void DrawSideRoom() const;

    Scene scene_ = Scene::Town;
    Player player_;
    std::vector<Rectangle> townObstacles_;
    std::vector<Rectangle> roomPlatforms_;
    Rectangle tavernDoor_;
    Rectangle roomExit_;
};
