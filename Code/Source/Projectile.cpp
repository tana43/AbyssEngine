#include "Projectile.h"
#include "Actor.h"
#include "Engine.h"
#include "RenderManager.h"
#include "DebugRenderer.h"

using namespace AbyssEngine;

void Projectile::Initialize(const std::shared_ptr<AbyssEngine::Actor> actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();
}

void Projectile::Update()
{
    //”ò‚Ñ“¹‹ï‚ÌŽËo‹——£
    const Vector3& pos = transform_->GetPosition();
    direction_.Normalize();
    const Vector3& velo = direction_ * speed_ * Time::deltaTime_;
    transform_->SetPosition(pos + velo);

#if _DEBUG
    Engine::renderManager_->debugRenderer_->DrawSphere(transform_->GetPosition(), radius_, Vector4(1, 0, 0, 1));
#endif // _DEBUG
}
