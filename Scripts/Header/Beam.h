#pragma once
#include "Projectile.h"
#include "ComputeParticleEmitter.h"

namespace AbyssEngine
{
    class SphereCollider;
    class AttackCollider;
    class BillboardRenderer;
    class TrailRenderer;
    //class ComputeParticleEmitter;
}

class Beam : public AbyssEngine::Projectile
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    void Update()override;

    void OnCollision(const std::shared_ptr<AbyssEngine::Collider>& collision, AbyssEngine::Collision::IntersectionResult result)override;

    void ParticleUpdate();

public:
    const float& GetAttackPoint() const { return attackPoint_; }
    void SetAttackPoint(const float& atk) { attackPoint_ = atk; }

    void SetColor(const AbyssEngine::Vector4& color);
    void SetWidth(const float& width);
    void SetIntensity(const float& bright);

    const std::shared_ptr<AbyssEngine::AttackCollider>& GetAttackCollider() { return attackCollider_; }

    AbyssEngine::ComputeParticleEmitter::EmitParameter& GetParticleEmitParameter() { return particleEmitPrameter_; }

private:
    //攻撃力
    float attackPoint_ = 1.0f;

    //球攻撃判定
    std::shared_ptr<AbyssEngine::AttackCollider> attackCollider_;

    //ビルボードポリゴン
    std::shared_ptr<AbyssEngine::BillboardRenderer> billboardRenderer_;

    //トレイルレンダラー
    std::shared_ptr<AbyssEngine::TrailRenderer> trailRenderer_;

    //パーティクル
    std::shared_ptr<AbyssEngine::ComputeParticleEmitter> particleEmitter_;

    AbyssEngine::ComputeParticleEmitter::EmitParameter particleEmitPrameter_;
    float particleSpeed_ = 20.0f;
    float particleAmplitude_ = 9.0f;//パーティクルの散らばり

    
};