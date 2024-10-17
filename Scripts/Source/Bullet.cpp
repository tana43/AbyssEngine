#include "Bullet.h"
#include "Actor.h"
#include "StaticMesh.h"
#include "Character.h"

using namespace AbyssEngine;

void Bullet::Initialize(const std::shared_ptr<Actor>& actor)
{
    Projectile::Initialize(actor);

    actor->AddComponent<StaticMesh>("./Assets/Models/Cube.glb");
    transform_->SetScaleFactor(0.03f);

    
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
            chara->ApplyDamage(param,&dmgResult);

            Actor::Destroy(actor_);
        }
    }
}
