#include "Projectile.h"
#include "Actor.h"
#include "Engine.h"
#include "RenderManager.h"
#include "DebugRenderer.h"
#include "SphereCollider.h"

using namespace AbyssEngine;

void Projectile::Initialize(const std::shared_ptr<Actor>& actor)
{
    ScriptComponent::Initialize(actor);

    collider_ = actor->AddComponent<SphereCollider>();
}

void Projectile::Update()
{
    //��ѓ���̎ˏo����
    const Vector3& pos = transform_->GetPosition();
    const Vector3& velo = direction_ * speed_ * Time::deltaTime_;
    transform_->SetPosition(pos + velo);

#if _DEBUG
    Engine::renderManager_->debugRenderer_->DrawSphere(transform_->GetPosition(), radius_, Vector4(1, 0, 0, 1));
#endif // _DEBUG

    //��������
    flashLifespan_ -= Time::deltaTime_;
    if (flashLifespan_ <= 0)
    {
        actor_->Destroy(actor_);
    }
}

void AbyssEngine::Projectile::SetRadius(const float& radius)
{
    radius_ = radius;
    collider_->SetRadius(radius);
}
