#include "Beam.h"
#include "Actor.h"
#include "StaticMesh.h"
#include "Character.h"
#include "ComputeParticleEmitter.h"
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
}

void Beam::Update()
{
    Projectile::Update();
}

void Beam::OnCollision(const std::shared_ptr<AbyssEngine::Collider>& collision, AbyssEngine::Collision::IntersectionResult result)
{
    //ƒ_ƒ[ƒWˆ—
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

void Beam::SetColor(const Vector4& color)
{
    billboardRenderer_->GetConstantBuffer()->data_.color_ = color;
    trailRenderer_->GetConstantBuffer()->data_.color_ = color;
}

void Beam::SetWidth(const float& width)
{
    trailRenderer_->SetWidth(width);
}
