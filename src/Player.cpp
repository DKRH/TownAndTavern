#include "Player.h"
#include <cmath>
#include <algorithm>

namespace
{
    constexpr float playerSpeed = 180.0f;
    constexpr float gravity = 1050.0f;
    constexpr float jumpSpeed = 390.0f;
}

Player::Player(float x, float y) : position_{ x, y } {}

Rectangle Player::GetBounds() const
{
    return { position_.x, position_.y, width_, height_ };
}

void Player::SetPosition(float x, float y)
{
    position_ = { x, y };
    velocity_ = { 0.0f, 0.0f };
    isGrounded_ = false;
}

void Player::UpdateCombat(float deltaTime)
{
    attackRequested_ = false;
    attackTimer_ = std::max(0.0f, attackTimer_ - deltaTime);
    dodgeTimer_ = std::max(0.0f, dodgeTimer_ - deltaTime);
    dodgeCooldown_ = std::max(0.0f, dodgeCooldown_ - deltaTime);
    invulnerabilityTimer_ = std::max(0.0f, invulnerabilityTimer_ - deltaTime);

    if (IsKeyPressed(KEY_Q))
        weapon_ = weapon_ == Weapon::Sword ? Weapon::Bow : Weapon::Sword;

    defending_ = weapon_ == Weapon::Sword &&
                 (IsKeyDown(KEY_K) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT));

    if (weapon_ == Weapon::Bow && dodgeCooldown_ <= 0.0f &&
        (IsKeyDown(KEY_K) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)))
    {
        dodgeTimer_ = 0.20f;
        dodgeCooldown_ = 0.65f;
        invulnerabilityTimer_ = 0.30f;
    }

    if (!defending_ && dodgeTimer_ <= 0.0f &&
        (IsKeyPressed(KEY_J) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)))
    {
        attackRequested_ = true;
        attackTimer_ = weapon_ == Weapon::Sword ? 0.22f : 0.12f;
    }
}

bool Player::ConsumeAttackRequest()
{
    const bool requested = attackRequested_;
    attackRequested_ = false;
    return requested;
}

void Player::KnockBackFrom(float sourceX, float distance)
{
    position_.x += position_.x + width_ / 2.0f >= sourceX ? distance : -distance;
    position_.x = std::clamp(position_.x, 0.0f, 800.0f - width_);
}

void Player::TakeDamage(int amount)
{
    health_ = std::max(0, health_ - amount);
    invulnerabilityTimer_ = 0.65f;
}

void Player::MoveAndCollide(float deltaX, float deltaY,
                            const std::vector<Rectangle>& obstacles)
{
    position_.x += deltaX;
    Rectangle bounds = GetBounds();
    for (const Rectangle obstacle : obstacles)
    {
        if (CheckCollisionRecs(bounds, obstacle))
        {
            if (deltaX > 0.0f) position_.x = obstacle.x - width_;
            if (deltaX < 0.0f) position_.x = obstacle.x + obstacle.width;
            bounds = GetBounds();
        }
    }

    position_.y += deltaY;
    bounds = GetBounds();
    for (const Rectangle obstacle : obstacles)
    {
        if (CheckCollisionRecs(bounds, obstacle))
        {
            if (deltaY > 0.0f)
            {
                position_.y = obstacle.y - height_;
                velocity_.y = 0.0f;
                isGrounded_ = true;
            }
            else if (deltaY < 0.0f)
            {
                position_.y = obstacle.y + obstacle.height;
                velocity_.y = 0.0f;
            }
            bounds = GetBounds();
        }
    }
}

void Player::UpdateTopDown(float deltaTime, const std::vector<Rectangle>& obstacles)
{
    Vector2 direction = { 0.0f, 0.0f };
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) direction.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) direction.x += 1.0f;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) direction.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) direction.y += 1.0f;

    const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0.0f)
    {
        direction.x /= length;
        direction.y /= length;
        if (std::abs(direction.x) > 0.1f) facing_ = direction.x > 0.0f ? 1.0f : -1.0f;
    }

    MoveAndCollide(direction.x * playerSpeed * deltaTime,
                   direction.y * playerSpeed * deltaTime, obstacles);
    position_.x = std::clamp(position_.x, 0.0f, 800.0f - width_);
    position_.y = std::clamp(position_.y, 0.0f, 384.0f - height_);
}

void Player::UpdateSideScroller(float deltaTime, const std::vector<Rectangle>& platforms)
{
    float direction = 0.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) direction -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) direction += 1.0f;
    if (direction != 0.0f) facing_ = direction > 0.0f ? 1.0f : -1.0f;
    if (dodgeTimer_ > 0.0f) position_.x -= facing_ * 430.0f * deltaTime;

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && isGrounded_)
    {
        velocity_.y = -jumpSpeed;
        isGrounded_ = false;
    }

    position_.x += direction * playerSpeed * deltaTime;
    Rectangle bounds = GetBounds();
    for (const Rectangle platform : platforms)
    {
        if (CheckCollisionRecs(bounds, platform))
        {
            if (direction > 0.0f) position_.x = platform.x - width_;
            if (direction < 0.0f) position_.x = platform.x + platform.width;
            bounds = GetBounds();
        }
    }

    const float previousBottom = position_.y + height_;
    velocity_.y += gravity * deltaTime;
    position_.y += velocity_.y * deltaTime;
    isGrounded_ = false;
    bounds = GetBounds();
    for (const Rectangle platform : platforms)
    {
        if (CheckCollisionRecs(bounds, platform) && velocity_.y >= 0.0f &&
            previousBottom <= platform.y + 2.0f)
        {
            position_.y = platform.y - height_;
            velocity_.y = 0.0f;
            isGrounded_ = true;
            bounds = GetBounds();
        }
    }

    position_.x = std::clamp(position_.x, 0.0f, 800.0f - width_);
}

void Player::Draw(Color color) const
{
    if (invulnerabilityTimer_ > 0.0f && static_cast<int>(invulnerabilityTimer_ * 24.0f) % 2 == 0)
        color = SKYBLUE;
    const Rectangle bounds = GetBounds();
    DrawRectangleRec(bounds, color);
    DrawRectangleLinesEx(bounds, 2.0f, MAROON);
    DrawCircle(static_cast<int>(position_.x + width_ / 2.0f),
               static_cast<int>(position_.y + 9.0f), 4.0f, BEIGE);

    const float centerX = position_.x + width_ / 2.0f;
    const float centerY = position_.y + height_ / 2.0f;
    if (weapon_ == Weapon::Sword && (defending_ || attackTimer_ > 0.0f))
    {
        const Color swordColor = defending_ ? SKYBLUE : LIGHTGRAY;
        DrawLineEx({ centerX + facing_ * 5.0f, centerY - 6.0f },
                   { centerX + facing_ * (defending_ ? 30.0f : 39.0f), centerY + 7.0f },
                   defending_ ? 7.0f : 5.0f, swordColor);
    }
    else if (weapon_ == Weapon::Bow)
    {
        DrawLineEx({ centerX + facing_ * 7.0f, centerY - 11.0f },
                   { centerX + facing_ * 7.0f, centerY + 11.0f }, 3.0f, BROWN);
    }
}
