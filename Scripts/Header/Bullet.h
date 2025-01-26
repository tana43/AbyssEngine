#pragma once
#include "Projectile.h"

namespace AbyssEngine
{
    class SphereCollider;
    class AttackCollider;
    class ComputeParticleEmitter;
    class TrailRenderer;
}

class Bullet : public AbyssEngine::Projectile
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    void Update()override;

    void OnCollision(const std::shared_ptr<AbyssEngine::Collider>& collision, AbyssEngine::Collision::IntersectionResult result)override;

    void HitTerrain()override;

public:
    const float& GetAttackPoint() const { return attackPoint_; }
    void SetAttackPoint(const float& atk) { attackPoint_ = atk; }

    const std::shared_ptr<AbyssEngine::AttackCollider>& GetAttackCollider() { return attackCollider_; }

private:
    //攻撃力
    float attackPoint_ = 1.0f;

    //球攻撃判定
    std::shared_ptr<AbyssEngine::AttackCollider> attackCollider_;

    //敵にヒットした際のエフェクト
    std::shared_ptr<AbyssEngine::ComputeParticleEmitter> hitEffect_;

    //トレイル
    std::shared_ptr<AbyssEngine::TrailRenderer> trailRenderer_;

    //地形に着弾時に生成されるパーティクル
    std::shared_ptr<AbyssEngine::ComputeParticleEmitter> terrainHitParticleEmitter_;
};

