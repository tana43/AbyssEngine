#include "SphereCollider.h"
#include "Engine.h"
#include "RenderManager.h"
#include "DebugRenderer.h"
#include "Transform.h"

using namespace AbyssEngine;

void AbyssEngine::SphereCollider::DrawDebug()
{
    Engine::renderManager_->debugRenderer_->DrawSphere(transform_->GetPosition(), radius_, debugColor_);
}
