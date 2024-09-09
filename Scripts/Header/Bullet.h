#pragma once
#include "Projectile.h"

class Bullet : public AbyssEngine::Projectile
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    void Update()override;

    void OnCollision(const std::shared_ptr<AbyssEngine::Collider>& collision, AbyssEngine::Collision::IntersectionResult result)override;

public:
    const float& GetAttackPoint() const { return attackPoint_; }
    void SetAttackPoint(const float& atk) { attackPoint_ = atk; }

private:
    //çUåÇóÕ
    float attackPoint_ = 1.0f;
};

