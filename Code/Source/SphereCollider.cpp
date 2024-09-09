#include "SphereCollider.h"
#include "Engine.h"
#include "RenderManager.h"
#include "DebugRenderer.h"
#include "Transform.h"

using namespace AbyssEngine;

void SphereCollider::Initialize(const std::shared_ptr<Actor>& actor)
{
    Collider::Initialize(actor);
}

void SphereCollider::DrawDebug()
{
#if _DEBUG
    Engine::renderManager_->debugRenderer_->DrawSphere(transform_->GetPosition(), radius_, debugColor_);
#endif // _DEBUG
}

bool SphereCollider::IntersectVsSphere(const std::shared_ptr<SphereCollider>& collider,Collision::IntersectionResult* result)
{
    Collision::IntersectSphereVsSphere(transform_->GetPosition(), radius_,
        collider->transform_->GetPosition(), collider->radius_, result);

    if (result->penetration > 0)return true;
    return false;
}
