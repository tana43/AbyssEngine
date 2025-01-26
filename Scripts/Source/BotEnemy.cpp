#include "BotEnemy.h"
#include "Actor.h"
#include "SkeletalMesh.h"
#include "Engine.h"
#include "RenderManager.h"
#include "StageManager.h"
#include "SceneManager.h"
#include "DebugRenderer.h"
#include "Gun.h"
#include "BehaviorTree.h"
#include "ActionDerived.h"
#include "SphereCollider.h"
#include "ComputeParticleEmitter.h"
#include "GameUIAdmin.h"

#include <memory>

#include "imgui/imgui.h"

using namespace AbyssEngine;

#define Ai_SelectRule BehaviorTree<BotEnemy>::SelectRule

void BotEnemy::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //初期化
    BaseEnemy::Initialize(actor);

    //HP設定
    Max_Health = 5.0f;
    health_ = Max_Health;

    //スケルタルメッシュを追加
    model_ = actor->AddComponent<SkeletalMesh>("./Assets/Models/Enemy/Bot/Bot_Idle.gltf");
    model_->GetModel()->primitiveConstants_->data_.minAmbient_ = 0.65f;
    model_->GetModel()->primitiveConstants_->data_.imageBasedLightingIntensity_ = 200.0f;

    model_->GetAnimator()->AppendAnimations(
        {
            "./Assets/Models/Enemy/Bot/Bot_Rolling.gltf",
            "./Assets/Models/Enemy/Bot/Bot_Walk.gltf",
            "./Assets/Models/Enemy/Bot/Bot_Jump.gltf",
            "./Assets/Models/Enemy/Bot/Bot_Search.gltf",
            "./Assets/Models/Enemy/Bot/Bot_Attack.gltf",
            "./Assets/Models/Enemy/Bot/Bot_Attack_Assult.gltf"
        },
        {
            "Rolling","Walk","Jump","Search","Attack","Attack_Assult"
        }
    );
    auto& animator = model_->GetAnimator();
    animator->GetAnimations()[static_cast<int>(AnimState::Rolling)]->SetAnimSpeed(1.5f);
    animator->GetAnimations()[static_cast<int>(AnimState::Attack)]->SetLoopFlag(false);
    animator->GetAnimations()[static_cast<int>(AnimState::Attack_Assult)]->SetLoopFlag(false);
    animator->GetAnimations()[static_cast<int>(AnimState::Jump)]->SetLoopFlag(false);
    animator->GetAnimations()[static_cast<int>(AnimState::Search)]->SetLoopFlag(false);

    //パラメータの設定
    transform_->SetScaleFactor(0.2f);
    Max_Horizontal_Speed = 2.0f;
    acceleration_ = 3.0f;

    reloadTimer_ = 0.0f;

    //視野角を設定
    fov_ = degreeFov_ / 360.0f;

    //攻撃設定
    canAttack_ = true;

    //銃
    gunComponentR_ = actor->AddComponent<Gun>();
    gunComponentR_->SetColliderTag(Collider::Tag::Enemy);

    //ビヘイビアツリー初期化
    BehaviorTreeInitialize();

    isTargetFind_ = false;

    //当たり判定初期化
    ColliderInitialize();

    //エフェクト
    destroyEffect_ = actor_->AddComponent<ComputeParticleEmitter>();
    destroyEffect_->SetEmitParamater("Bot_Destroy");
}

void BotEnemy::BehaviorTreeInitialize()
{
    //ビヘイビアツリー
    aiTree_ = actor_->AddComponent<BehaviorTree<BotEnemy>>();
    aiTree_->SetOwner(std::static_pointer_cast<BotEnemy>(shared_from_this()));

    //BehaviorTreeを構築
    aiTree_->AddNode("", "Root", 0, Ai_SelectRule::Priority,nullptr,nullptr);
    {

        //戦闘
        aiTree_->AddNode("Root", "Battle", 0, Ai_SelectRule::Sequence, new BotBattleJudgment(this), nullptr);
        {
            //戦闘ノード
            aiTree_->AddNode("Battle", "Attack", 0, Ai_SelectRule::Non, new AttackJudgment(this), new BotAttackAction(this));
            aiTree_->AddNode("Battle", "Dodge", 1, Ai_SelectRule::Non, nullptr, new BotSideDodgeAction(this));
            //aiTree_->AddNode("Battle", "Dodge", 1, Ai_SelectRule::Non, new DodgeJudgment(this), new BotSideDodgeAction(this));
        }

        //偵察
        aiTree_->AddNode("Root", "Scout", 1, Ai_SelectRule::Sequence, nullptr, nullptr);
        {
            //偵察ノード
            aiTree_->AddNode("Scout", "Wonder", 0, Ai_SelectRule::Non, nullptr, new BotWonderActioin(this));
            aiTree_->AddNode("Scout", "Idle", 1, Ai_SelectRule::Non, nullptr, new BotIdleAction(this));
        }
    }
}

void BotEnemy::Update()
{
    BaseEnemy::Update();

    ReloadUpdate();
}

void BotEnemy::DrawImGui()
{
    BaseEnemy::DrawImGui();

    if (ImGui::TreeNode("Bot Enemy"))
    {
        ImGui::DragFloat("LockOn Time",&LockOn_Time,0.05f);
        ImGui::DragFloat("LockOn Shot Time",&LockOn_Shot_Time, 0.05f);
        ImGui::DragFloat("Reload Time",&Atk_Reload_Time, 0.05f);
        ImGui::DragFloat("Reload Time",&Atk_Reload_Time, 0.05f);

        ImGui::TreePop();
    }
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
        if (!targetActor_)return false;
    }

    //ターゲットが索敵範囲内に入っているか
    const Vector3 myPos = transform_->GetPosition();
    const Vector3 targetPos = targetActor_->GetTransform()->GetPosition();
    Vector3 toTarget = targetPos - myPos;
    float dist = toTarget.Length();
    if (dist < searchAreaRadius_)
    {
        //レイキャストで障害物がターゲットまでにないか判定
        Vector3 hitPos,hitNormal;
        const auto& stage = Engine::stageManager_->GetActiveStage().lock();
        if (!stage->RayCast(
            myPos,
            targetPos,
            hitPos,
            hitNormal
        ))
        {
#if 0
            //内積値から視野角を考慮
            toTarget.Normalize();
            float dot = toTarget.Dot(transform_->GetForward());

            if (dot > 1.0f - fov_)
            {
                //視界内に入っている
                return true;
            }
#else
            isTargetFind_ = true;

            return true;
#endif // 0
        }
    }

    return false;
}

bool BotEnemy::SearchTargetWithimRange(float range)
{
    //ターゲットが指定範囲内にいるか判定
    const Vector3 pos = transform_->GetPosition();
    const Vector3 targetPos = targetActor_->GetTransform()->GetPosition();
    float distSq = Vector3::DistanceSquared(pos, targetPos);

    if (distSq < range * range)
    {
        return true;
    }

    return false;
}

void BotEnemy::LockOn()
{
    const auto& chara = targetActor_->GetComponent<Character>();
    aimPosition_ = chara->GetCenterPos();
}

void BotEnemy::Shot()
{
    //銃口位置を設定
    gunComponentR_->SetMuzzlePos(transform_->GetPosition() + Vector3(0, 0.3f, 0));

    const Vector3 pos = transform_->GetPosition();
    Vector3 toAimPos = aimPosition_ - pos;
    toAimPos.Normalize();
    gunComponentR_->Shot(toAimPos,nullptr);

    //攻撃を不可に
    canAttack_ = false;

    //リロードフラグをオン
    reloadNow_ = true;
}

void BotEnemy::ReloadUpdate()
{
    //リロードしているか
    if (!reloadNow_)return;

    reloadTimer_ += actor_->GetDeltaTime();

    if (reloadTimer_ > Atk_Reload_Time)
    {
        reloadNow_ = false;
        canAttack_ = true;
        reloadTimer_ = 0.0f;
    }
}

void BotEnemy::ColliderInitialize()
{
    //collider_ = AddHitCollider(Vector3(0, 0.2f, 0), 0.3f);
    hitCollider_ = AddHitCollider(Vector3(0, 0.2f, 0), 0.3f);
}

void BotEnemy::OnDead()
{
    destroyEffect_->EmitParticle();
    
    Engine::sceneManager_->GetActiveScene().DestroyActor(actor_);
}

void BotEnemy::OnDamaged()
{
    //ダメージを与えられたことをUIに表示
    const auto& actor = Engine::sceneManager_->GetActiveScene().Find("GameUI");
    if (const auto& p = actor.lock())
    {
        p->GetComponent<GameUIAdmin>()->BulletHit();
    }
}

const std::shared_ptr<AbyssEngine::Animator>& BotEnemy::GetAnimator() const
{
    return model_->GetAnimator();
}
