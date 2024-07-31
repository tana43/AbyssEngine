#include "BaseEnemy.h"
#include "Actor.h"
#include "Engine.h"
#include "RenderManager.h"
#include "DebugRenderer.h"

using namespace AbyssEngine;

void BaseEnemy::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //初期化
    Character::Initialize(actor);

    //エネミータグ付け
    Character::ReplaceTag(Character::Tag_Enemy);
}

void BaseEnemy::DrawDebug()
{
    // 縄張り範囲をデバッグ円柱描画
    Engine::renderManager_->debugRenderer_->DrawCylinder(territoryOrigin_, territoryRange_, 1.0f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
}

void BaseEnemy::MoveToTarget()
{
    Vector3 moveVec = targetPosition_ - transform_->GetTransform()->GetPosition();
    moveVec.y = 0.0f;
    moveVec.Normalize();
    moveVec_ = moveVec;
}

void BaseEnemy::SetRandomTargetPosition()
{
    float theta = Math::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
    float range = Math::RandomRange(0.0f, territoryRange_);
    targetPosition_.x = territoryOrigin_.x + sinf(theta) * range;
    targetPosition_.y = territoryOrigin_.y;
    targetPosition_.z = territoryOrigin_.z + cosf(theta) * range;
}

void BaseEnemy::SetTerritory(const AbyssEngine::Vector3& origin, float range)
{
    territoryOrigin_ = origin;
    territoryRange_ = range;
}
