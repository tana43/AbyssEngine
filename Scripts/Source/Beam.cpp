#include "Beam.h"
#include "Actor.h"
#include "StaticMesh.h"
#include "Character.h"
//#include "ComputeParticleEmitter.h"
#include "TrailRenderer.h"
#include "BillboardRenderer.h"

using namespace AbyssEngine;

void Beam::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    Projectile::Initialize(actor);

    billboardRenderer_ = actor->AddComponent<BillboardRenderer>("./Assets/Effects/Texture/Particle04_bokashi_soft.png");
    billboardRenderer_->SetIntensity(2.0f);

    trailRenderer_ = actor->AddComponent<TrailRenderer>("./Assets/Effects/TrailTexture/Beam.png");

    straightParticleEmitter_ = actor->AddComponent<ComputeParticleEmitter>();
    hitParticleEmitter_ = actor->AddComponent<ComputeParticleEmitter>();
    hitFireParticleEmitter_ = actor->AddComponent<ComputeParticleEmitter>();

    transform_->SetScaleFactor(0.03f);

    //パーティクルエミッター設定
    straightParticleEmitPrameter_.lifespan_ = 0.5f;
    straightParticleEmitPrameter_.texType_ = 3;
    straightParticleEmitPrameter_.emitNum_ = 12;
    straightParticleEmitPrameter_.color_ = { 0,0.25f,1.0f,1.0f };
    straightParticleEmitPrameter_.intensity_ = 100.0f;
    straightParticleEmitPrameter_.colorAmplitud_ = { 0,0,0,0 };
    straightParticleEmitPrameter_.positionAmplitude_ = {2.0f,2.0f,2.0f};
    straightParticleEmitPrameter_.scaleInit_ = { 0.4f,0.4f };
    straightParticleEmitPrameter_.rotationAmplitude_ = { 0,0,180.0f };
    //particleEmitter_->SetEmitParamater(particleEmitPrameter_);
}

void Beam::Update()
{
    Projectile::Update();

    ParticleUpdate();
}

void Beam::OnCollision(const std::shared_ptr<AbyssEngine::Collider>& collision, AbyssEngine::Collision::IntersectionResult result)
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

void Beam::ParticleUpdate()
{
    //パーティクルの動きを設定
    straightParticleEmitPrameter_.velocity_ = -direction_ * particleSpeed_;

    //右方向ベクトルと上方向ベクトルからパーティクルの散らばりを算出
    const Vector3 right = direction_.Cross(Vector3::Up);
    const Vector3 up = right.Cross(direction_);
    straightParticleEmitPrameter_.velocityAmplitude_ = right * particleAmplitude_ + up * particleAmplitude_;

    //パーティクル生成
    straightParticleEmitter_->EmitParticle(straightParticleEmitPrameter_);
}

void Beam::SetColor(const Vector4& color)
{
    billboardRenderer_->GetConstantBuffer()->data_.color_ = color;
    trailRenderer_->SetColor(color);
}

void Beam::SetWidth(const float& width)
{
    trailRenderer_->SetWidth(width);
}

void Beam::SetIntensity(const float& bright)
{
    trailRenderer_->SetIntensity(bright);
    straightParticleEmitPrameter_.intensity_ = bright;
}

void Beam::SetParticleIntensity(const float& intensity)
{
    straightParticleEmitPrameter_.intensity_ = intensity;
}
