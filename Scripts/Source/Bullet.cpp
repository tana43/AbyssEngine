#include "Bullet.h"
#include "Actor.h"
#include "StaticMesh.h"
#include "Character.h"
#include "TrailRenderer.h"
#include "ComputeParticleEmitter.h"

using namespace AbyssEngine;

void Bullet::Initialize(const std::shared_ptr<Actor>& actor)
{
    Projectile::Initialize(actor);

    //actor->AddComponent<StaticMesh>("./Assets/Models/Cube.glb");
    //transform_->SetScaleFactor(0.03f);

    trailRenderer_ = actor_->AddComponent<TrailRenderer>("./Assets/Effects/TrailTexture/Bullet_02.png");

    hitEffect_ = actor_->AddComponent<ComputeParticleEmitter>();
    hitEffect_->SetEmitParamater("Bullet_Hit_Enemy");

    terrainHitParticleEmitter_ = actor_->AddComponent<ComputeParticleEmitter>();
    terrainHitParticleEmitter_->SetEmitParamater("Bullet_Hit_Spark");
}

void Bullet::Update()
{
    Projectile::Update();
}

void Bullet::OnCollision(const std::shared_ptr<Collider>& collision, Collision::IntersectionResult result)
{
    //ダメージ処理
    if (const auto& actor = collision->GetActor()->GetParent().lock())
    {
        if (const auto& chara = actor->GetComponent<Character>())
        {
            Character::DamageResult dmgResult;
            Character::AttackParameter param;
            param.power_ = attackPoint_;
            if (chara->ApplyDamage(param, &dmgResult))
            {
                //ヒットエフェクト再生
                hitEffect_->EmitParticle();
            }

            Actor::Destroy(actor_);
        }
    }
}

void Bullet::HitTerrain()
{
    //地形へヒットした際のエフェクト再生
    terrainHitParticleEmitter_->EmitParticle();
}
