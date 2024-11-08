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

    trailRenderer_ = actor->AddComponent<TrailRenderer>("./Assets/Effects/TrailTexture/Beam.png");

    particleEmitter_ = actor->AddComponent<ComputeParticleEmitter>();

    transform_->SetScaleFactor(0.03f);

    //パーティクルエミッター設定
    particleEmitPrameter_.lifespan_ = 0.5f;
    particleEmitPrameter_.texType_ = 3;
    particleEmitPrameter_.emitNum_ = 12;
    particleEmitPrameter_.color_ = { 0,0.25f,1.0f,1.0f };
    particleEmitPrameter_.intensity_ = 10.0f;
    particleEmitPrameter_.colorAmplitud_ = { 0,0.1f,1.0f,0 };
    particleEmitPrameter_.positionAmplitude_ = {2.0f,2.0f,2.0f};
    particleEmitPrameter_.scaleInit_ = { 0.4f,0.4f };
    particleEmitPrameter_.rotationAmplitude_ = { 0,0,180.0f };
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

            Actor::Destroy(actor_);
        }
    }
}

void Beam::ParticleUpdate()
{
    //パーティクルの動きを設定
    particleEmitPrameter_.velocity_ = -direction_ * particleSpeed_;

    //右方向ベクトルと上方向ベクトルからパーティクルの散らばりを算出
    const Vector3 right = direction_.Cross(Vector3::Up);
    const Vector3 up = right.Cross(direction_);
    particleEmitPrameter_.velocityAmplitude_ = right * particleAmplitude_ + up * particleAmplitude_;

    //パーティクル生成
    particleEmitter_->EmitParticle(particleEmitPrameter_);
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

void Beam::SetBrightness(const float& bright)
{
    trailRenderer_->SetBrightness(bright);
    particleEmitPrameter_.intensity_ = bright;
}
