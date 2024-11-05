#include "Projectile.h"
#include "Actor.h"
#include "Engine.h"
#include "RenderManager.h"
#include "DebugRenderer.h"
#include "SphereCollider.h"
#include "GameCollider.h"
#include "SceneManager.h"

using namespace AbyssEngine;

void Projectile::Initialize(const std::shared_ptr<Actor>& actor)
{
    ScriptComponent::Initialize(actor);

    atkCollider_ = AddAttackCollider(Vector3::Zero, radius_);
    //collider_ = actor->AddComponent<SphereCollider>();
}

void Projectile::Update()
{
    if (isHoming_)
    {
        //ホーミング更新
        HomingUpdate();
    }
    
    //移動更新
    MoveUpdate();
    

#if _DEBUG
    Engine::renderManager_->debugRenderer_->DrawSphere(transform_->GetPosition(), radius_, Vector4(1, 0, 0, 1));
#endif // _DEBUG

    LifeTimeUpdate();
}

void AbyssEngine::Projectile::SetRadius(const float& radius)
{
    radius_ = radius;
    atkCollider_->SetRadius(radius);
}

void AbyssEngine::Projectile::LifeTimeUpdate()
{
    //寿命処理
    lifespan_ -= actor_->GetDeltaTime();
    if (lifespan_ <= 0)
    {
        actor_->Destroy(actor_);
    }
}

void AbyssEngine::Projectile::MoveUpdate()
{
    //飛び道具の射出距離
    const Vector3& pos = transform_->GetPosition();
    const Vector3& velo = direction_ * speed_ * actor_->GetDeltaTime();
    transform_->SetPosition(pos + velo);
}

void AbyssEngine::Projectile::HomingUpdate()
{
    //ターゲットを検索
    const auto& t = Engine::sceneManager_->GetActiveScene().FindByTag(targetTag_);
    if (const auto& target = t.lock())
    {
        //自分の向きと敵の向きから進行角度を補完していく
        const Vector3 pos = transform_->GetPosition();
        const Vector3 targetPos = target->GetTransform()->GetPosition();

        Vector3 dir = targetPos - pos;
        dir.Normalize();

        float t = homingStrength_ * Time::GetDeltaTime();
        t = std::clamp(t, 0.0f, 1.0f);

        direction_ = Vector3::Lerp(direction_, dir, t);
    }
}
