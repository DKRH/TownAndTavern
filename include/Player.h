#pragma once

#include "raylib.h"
#include <vector>

class Player
{
public:
    enum class Weapon { Sword, Bow };

    Player(float x, float y);
    void UpdateTopDown(float deltaTime, const std::vector<Rectangle>& obstacles);
    void UpdateSideScroller(float deltaTime, const std::vector<Rectangle>& platforms);
    void Draw(Color color = GOLD) const;
    Rectangle GetBounds() const;
    void SetPosition(float x, float y);
    void UpdateCombat(float deltaTime);
    bool ConsumeAttackRequest();
    Weapon GetWeapon() const { return weapon_; }
    bool IsDefending() const { return defending_; }
    bool IsInvulnerable() const { return invulnerabilityTimer_ > 0.0f; }
    int GetHealth() const { return health_; }
    int GetMaxHealth() const { return maxHealth_; }
    float GetFacing() const { return facing_; }
    void KnockBackFrom(float sourceX, float distance);
    void TakeDamage(int amount);
    void RestoreHealth() { health_ = maxHealth_; }

private:
    void MoveAndCollide(float deltaX, float deltaY, const std::vector<Rectangle>& obstacles);
    Vector2 position_;
    Vector2 velocity_ = { 0.0f, 0.0f };
    float width_ = 28.0f;
    float height_ = 36.0f;
    bool isGrounded_ = false;
    float facing_ = 1.0f;
    Weapon weapon_ = Weapon::Sword;
    bool defending_ = false;
    bool attackRequested_ = false;
    float attackTimer_ = 0.0f;
    float dodgeTimer_ = 0.0f;
    float dodgeCooldown_ = 0.0f;
    float invulnerabilityTimer_ = 0.0f;
    int health_ = 5;
    int maxHealth_ = 5;
};
