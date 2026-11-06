#pragma once

#include "raylib.h"

class Creature
{
public:
    Creature(float x, float y);
    bool Update(float deltaTime, Rectangle playerBounds);
    void TakeDamage(int amount);
    void KnockBackFrom(float sourceX, float distance);
    bool IsAlive() const { return health_ > 0; }
    Rectangle GetBounds() const;
    void Draw() const;

private:
    Vector2 position_;
    float width_ = 34.0f;
    float height_ = 38.0f;
    int health_ = 5;
    int maxHealth_ = 5;
    float attackCooldown_ = 0.7f;
    float attackWindup_ = 0.0f;
    float hurtTimer_ = 0.0f;
};
