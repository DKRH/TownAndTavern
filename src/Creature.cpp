#include "Creature.h"
#include <algorithm>
#include <cmath>

Creature::Creature(float x, float y) : position_{ x, y } {}

Rectangle Creature::GetBounds() const
{
    return { position_.x, position_.y, width_, height_ };
}

bool Creature::Update(float deltaTime, Rectangle playerBounds)
{
    if (!IsAlive()) return false;

    attackCooldown_ = std::max(0.0f, attackCooldown_ - deltaTime);
    hurtTimer_ = std::max(0.0f, hurtTimer_ - deltaTime);
    const float playerCenterX = playerBounds.x + playerBounds.width / 2.0f;
    const float centerX = position_.x + width_ / 2.0f;
    const float distanceX = playerCenterX - centerX;
    const float verticalDistance = std::abs((playerBounds.y + playerBounds.height) -
                                            (position_.y + height_));

    if (attackWindup_ > 0.0f)
    {
        attackWindup_ -= deltaTime;
        if (attackWindup_ <= 0.0f) return true;
    }

    if (verticalDistance < 28.0f)
    {
        if (std::abs(distanceX) > 29.0f)
            position_.x += (distanceX > 0.0f ? 1.0f : -1.0f) * 72.0f * deltaTime;
        else if (attackCooldown_ <= 0.0f && attackWindup_ <= 0.0f)
        {
            attackWindup_ = 0.24f;
            attackCooldown_ = 1.05f;
        }
    }
    return false;
}

void Creature::TakeDamage(int amount)
{
    health_ = std::max(0, health_ - amount);
    hurtTimer_ = 0.16f;
}

void Creature::KnockBackFrom(float sourceX, float distance)
{
    const float centerX = position_.x + width_ / 2.0f;
    position_.x += centerX >= sourceX ? distance : -distance;
    position_.x = std::clamp(position_.x, 0.0f, 800.0f - width_);
}

void Creature::Draw() const
{
    if (!IsAlive()) return;
    const Rectangle bounds = GetBounds();
    const Color bodyColor = hurtTimer_ > 0.0f ? RAYWHITE : Color{ 159, 69, 103, 255 };
    DrawRectangleRec(bounds, bodyColor);
    DrawRectangleLinesEx(bounds, 2.0f, DARKPURPLE);
    DrawCircle(static_cast<int>(position_.x + 10.0f), static_cast<int>(position_.y + 13.0f), 3.0f, YELLOW);
    DrawCircle(static_cast<int>(position_.x + 24.0f), static_cast<int>(position_.y + 13.0f), 3.0f, YELLOW);
    const float barWidth = 40.0f;
    DrawRectangle(static_cast<int>(position_.x - 3.0f), static_cast<int>(position_.y - 10.0f), 40, 5, DARKGRAY);
    DrawRectangle(static_cast<int>(position_.x - 3.0f), static_cast<int>(position_.y - 10.0f),
                  static_cast<int>(barWidth * health_ / static_cast<float>(maxHealth_)), 5, RED);
}
