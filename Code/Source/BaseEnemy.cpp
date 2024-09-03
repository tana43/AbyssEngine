#include "BaseEnemy.h"
#include "Actor.h"
#include "Engine.h"
#include "RenderManager.h"
#include "DebugRenderer.h"
#include "SceneManager.h"

using namespace AbyssEngine;

void BaseEnemy::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //初期化
    Character::Initialize(actor);

    //エネミータグ付け
    Character::ReplaceTag(Character::Tag_Enemy);

    //テリトリー初期値設定
    territoryOrigin_ = transform_->GetPosition();

    //各行動を可能状態に
    canAttack_ = true;
    canDodge_ = false;

    //ターゲット取得
    const auto& target = Engine::sceneManager_->GetActiveScene().Find("Player");
    if (const auto& t = target.lock())
    {
        targetActor_ = t;
    }
}

void BaseEnemy::UpdateBegin()
{
    //移動ベクトルをリセット
    moveVec_ = {};
}

void BaseEnemy::DrawDebug()
{
#if _DEBUG
    // 縄張り範囲をデバッグ円柱描画
    Engine::renderManager_->debugRenderer_->DrawCylinder(territoryOrigin_, territoryRange_, 0.5f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
#endif // _DEBUG
}

bool BaseEnemy::MoveToTarget()
{
    Vector3 toTarget = targetPosition_ - transform_->GetTransform()->GetPosition();
    Vector3 moveVec = toTarget;
    moveVec.y = 0.0f;
    moveVec.Normalize();
    moveVec_ = moveVec;

    //目的地についているか判定
    float distSq = toTarget.LengthSquared();
    float radiusSq = goalRadius_ * goalRadius_;
    if (distSq < radiusSq)
    {
        return true;
    }

    return false;
}

void BaseEnemy::SideMove(const AbyssEngine::Vector3& centerPos ,bool& moveRight)
{
    //壁に当たっているなら反対方向へ
    if (hitWall_)
    {
        moveRight = !moveRight;
    }

    //引数の座標から見て横へ動く
    //外積を使用
    Vector3 toCenter = centerPos - transform_->GetPosition();
    toCenter.Normalize();
    //左右どちらに移動するかに合わせて外積に使用するベクトルを変更  
    const Vector3 ud = moveRight ? Vector3(0, -1, 0) : Vector3(0, 1, 0);
    const Vector3 cross = toCenter.Cross(ud);

    moveVec_ = cross;
}

void BaseEnemy::SetRandomTargetPosition()
{
    float theta = Math::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
    float range = Math::RandomRange(0.0f, territoryRange_);
    float a = cosf(theta);
    targetPosition_.x = territoryOrigin_.x + sinf(theta) * range;
    targetPosition_.y = territoryOrigin_.y;
    targetPosition_.z = territoryOrigin_.z + cosf(theta) * range;
}

void BaseEnemy::SetTerritory(const AbyssEngine::Vector3& origin, float range)
{
    territoryOrigin_ = origin;
    territoryRange_ = range;
}
