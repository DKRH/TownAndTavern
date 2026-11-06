#include "Game.h"

int main()
{
    InitWindow(800, 450, "Town and Tavern - C++ RPG Learning Project");
    SetTargetFPS(60);

    Game game;
    while (!WindowShouldClose())
    {
        const float deltaTime = GetFrameTime();
        game.Update(deltaTime);
        game.Draw();
    }

    CloseWindow();
    return 0;
}
