#include "SphereCollider.h"
#include "Engine.h"
#include "RenderManager.h"
#include "DebugRenderer.h"
#include "Transform.h"

using namespace AbyssEngine;

void AbyssEngine::SphereCollider::DrawDebug()
{
#if _DEBUG
    Engine::renderManager_->debugRenderer_->DrawSphere(transform_->GetPosition(), radius_, debugColor_);
#endif // _DEBUG

}
