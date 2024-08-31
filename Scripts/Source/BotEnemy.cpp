#include "BotEnemy.h"
#include "Actor.h"
#include "SkeletalMesh.h"
#include "Engine.h"
#include "RenderManager.h"
#include "StageManager.h"
#include "SceneManager.h"
#include "DebugRenderer.h"

using namespace AbyssEngine;

void BotEnemy::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //初期化
    BaseEnemy::Initialize(actor);

    //スケルタルメッシュを追加
    model_ = actor->AddComponent<SkeletalMesh>("./Assets/Models/Enemy/Bot_Idle.glb");
    model_->GetAnimator()->AppendAnimations(
        {
            "./Assets/Models/Enemy/Bot_Rolling.glb",
            "./Assets/Models/Enemy/Bot_Walk.glb",
            "./Assets/Models/Enemy/Bot_Jump.glb",
            "./Assets/Models/Enemy/Bot_Search.glb",
            "./Assets/Models/Enemy/Bot_Attack.glb",
            "./Assets/Models/Enemy/Bot_Attack_Assult.glb"
        },
        {
            "Rolling","Walk","Jump","Search","Attack","Attack_Assult"
        }
    );

    //パラメータの設定
    transform_->SetScaleFactor(0.2f);
    Max_Horizontal_Speed = 2.0f;
    acceleration_ = 0.5f;

    //視野角を設定
    fov_ = degreeFov_ / 360.0f;

    //攻撃設定
    canAttack_ = true;
}

void BotEnemy::Update()
{
    BaseEnemy::Update();
}

void BotEnemy::DrawDebug()
{
    BaseEnemy::DrawDebug();

#if _DEBUG
    // 索敵範囲をデバッグ円柱描画
    Engine::renderManager_->debugRenderer_->DrawCylinder(transform_->GetPosition(), searchAreaRadius_, 0.5f, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.7f));
#endif // _DEBUG
}

bool BotEnemy::SearchTarget()
{
    //ターゲット取得
    if (!targetActor_)
    {
        targetActor_ = Engine::sceneManager_->GetActiveScene().Find("Player").lock();
    }

    //ターゲットが索敵範囲内に入っているか
    const Vector3 myPos = transform_->GetPosition();
    const Vector3 targetPos = targetActor_->GetTransform()->GetPosition();
    Vector3 toTarget = targetPos - myPos;
    float dist = toTarget.Length();
    if (dist < searchAreaRadius_)
    {
        //レイキャストで視界にターゲットが入っているか判定
        Vector3 hitPos,hitNormal;
        auto& stage = StageManager::Instance().GetActiveStage();
        if (stage->RayCast(
            myPos,
            targetPos,
            hitPos,
            hitNormal
        ))
        {
            //内積値から視野角を考慮
            toTarget.Normalize();
            float dot = toTarget.Dot(transform_->GetForward());

            if (dot > 1.0f - fov_)
            {
                //視界内に入っている
                return true;
            }
        }
    }

    return false;
}
