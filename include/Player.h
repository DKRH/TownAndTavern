#pragma once

#include "raylib.h"
#include <vector>

class Player
{
public:
    Player(float x, float y);
    void UpdateTopDown(float deltaTime, const std::vector<Rectangle>& obstacles);
    void UpdateSideScroller(float deltaTime, const std::vector<Rectangle>& platforms);
    void Draw(Color color = GOLD) const;
    Rectangle GetBounds() const;
    void SetPosition(float x, float y);

private:
    void MoveAndCollide(float deltaX, float deltaY, const std::vector<Rectangle>& obstacles);
    Vector2 position_;
    Vector2 velocity_ = { 0.0f, 0.0f };
    float width_ = 28.0f;
    float height_ = 36.0f;
    bool isGrounded_ = false;
};
