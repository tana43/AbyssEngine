#pragma once
#include "Projectile.h"

namespace AbyssEngine
{
    SphereCollider;
    AttackCollider;
}

class Bullet : public AbyssEngine::Projectile
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    void Update()override;

    void OnCollision(const std::shared_ptr<AbyssEngine::Collider>& collision, AbyssEngine::Collision::IntersectionResult result)override;

public:
    const float& GetAttackPoint() const { return attackPoint_; }
    void SetAttackPoint(const float& atk) { attackPoint_ = atk; }

    const std::shared_ptr<AbyssEngine::AttackCollider>& GetAttackCollider() { return attackCollider_; }

private:
    //UŒ‚—Í
    float attackPoint_ = 1.0f;

    //‹…UŒ‚”»’è
    std::shared_ptr<AbyssEngine::AttackCollider> attackCollider_;
};

