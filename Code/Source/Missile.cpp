#include "Missile.h"
#include "Actor.h"
#include "StaticMesh.h"
#include "Character.h"
//#include "ComputeParticleEmitter.h"
#include "TrailRenderer.h"
#include "BillboardRenderer.h"

using namespace AbyssEngine;

void Missile::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    Projectile::Initialize(actor);

    billboardRenderer_ = actor->AddComponent<BillboardRenderer>("./Assets/Effects/Texture/Particle04_bokashi_soft.png");
    billboardRenderer_->SetIntensity(2.0f);

    trailRenderer_ = actor->AddComponent<TrailRenderer>("./Assets/Effects/TrailTexture/Beam.png");

    straightParticleEmitter_ = actor->AddComponent<ComputeParticleEmitter>();
    hitParticleEmitter_ = actor->AddComponent<ComputeParticleEmitter>();
    hitFireParticleEmitter_ = actor->AddComponent<ComputeParticleEmitter>();
    terrainHitParticleEmitter_ = actor->AddComponent<ComputeParticleEmitter>();

    transform_->SetScaleFactor(0.5f);

    //パーティクルエミッター設定
    straightParticleEmitPrameter_.lifespan_ = 0.5f;
    straightParticleEmitPrameter_.lifespanAmplitude_ = 0.2f;
    straightParticleEmitPrameter_.texType_ = 5;
    straightParticleEmitPrameter_.emitNum_ = 10;
    straightParticleEmitPrameter_.color_ = { 1.0f,0.4f,0.0f,1.0f };
    straightParticleEmitPrameter_.intensity_ = 3.0f;
    straightParticleEmitPrameter_.colorAmplitud_ = { 0,0,0,0 };
    straightParticleEmitPrameter_.positionAmplitude_ = { 0.0f,0.0f,0.0f };
    straightParticleEmitPrameter_.scaleInit_ = { 2.0f,2.0f };

    radius_ = 1.0f;
    //particleEmitter_->SetEmitParamater(particleEmitPrameter_);
}

void Missile::Update()
{
    Projectile::Update();

    ParticleUpdate();
}

void Missile::OnCollision(const std::shared_ptr<AbyssEngine::Collider>& collision, AbyssEngine::Collision::IntersectionResult result)
{
    //ダメージ処理
    if (const auto& actor = collision->GetActor()->GetParent().lock())
    {
        if (const auto& chara = actor->GetComponent<Character>())
        {
            Character::DamageResult dmgResult;
            Character::AttackParameter param;
            param.power_ = attackPoint_;
            chara->ApplyDamage(param, &dmgResult);

            //パーティクルヒットエフェクト再生
            hitFireParticleEmitter_->EmitParticle();
            hitParticleEmitter_->EmitParticle();

            Actor::Destroy(actor_);
        }
    }
}

void Missile::ParticleUpdate()
{
    //パーティクルの動きを設定
    straightParticleEmitPrameter_.velocity_ = -transform_->GetForward() * particleSpeed_;

    //右方向ベクトルと上方向ベクトルからパーティクルの散らばりを算出
    const Vector3 right = transform_->GetRight();
    const Vector3 up = transform_->GetUp();
    straightParticleEmitPrameter_.velocityAmplitude_ = right * particleAmplitude_ + up * particleAmplitude_;

    //パーティクル生成
    straightParticleEmitter_->EmitParticle(straightParticleEmitPrameter_);
}

void Missile::SetColor(const Vector4& color)
{
    billboardRenderer_->GetConstantBuffer()->data_.color_ = color;
    trailRenderer_->SetColor(color);
}

void Missile::SetWidth(const float& width)
{
    trailRenderer_->SetWidth(width);
}

void Missile::SetIntensity(const float& bright)
{
    trailRenderer_->SetIntensity(bright);
    billboardRenderer_->SetIntensity(bright);
    //straightParticleEmitPrameter_.intensity_ = bright;
}

void Missile::SetParticleIntensity(const float& intensity)
{
    straightParticleEmitPrameter_.intensity_ = intensity;
}

void Missile::HitTerrain()
{
    //エフェクト生成
    terrainHitParticleEmitter_->EmitParticle();
}
