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

#include <memory>

#include "imgui/imgui.h"

using namespace AbyssEngine;

#define Ai_SelectRule BehaviorTree<BotEnemy>::SelectRule

void BotEnemy::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //������
    BaseEnemy::Initialize(actor);

    //�X�P���^�����b�V����ǉ�
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
    auto& animator = model_->GetAnimator();
    animator->GetAnimations()[static_cast<int>(AnimState::Rolling)]->SetAnimSpeed(1.5f);
    animator->GetAnimations()[static_cast<int>(AnimState::Attack)]->SetLoopFlag(false);
    animator->GetAnimations()[static_cast<int>(AnimState::Attack_Assult)]->SetLoopFlag(false);
    animator->GetAnimations()[static_cast<int>(AnimState::Jump)]->SetLoopFlag(false);
    animator->GetAnimations()[static_cast<int>(AnimState::Search)]->SetLoopFlag(false);

    //�p�����[�^�̐ݒ�
    transform_->SetScaleFactor(0.2f);
    Max_Horizontal_Speed = 2.0f;
    acceleration_ = 3.0f;

    reloadTimer_ = 0.0f;

    //����p��ݒ�
    fov_ = degreeFov_ / 360.0f;

    //�U���ݒ�
    canAttack_ = true;

    //�e
    gunComponent_ = actor->AddComponent<Gun>();
    gunComponent_->SetColliderTag(Collider::Tag::Enemy);

    //�r�w�C�r�A�c���[������
    BehaviorTreeInitialize();

    isTargetFind_ = false;

    //�����蔻�菉����
    ColliderInitialize();
}

void BotEnemy::BehaviorTreeInitialize()
{
    //�r�w�C�r�A�c���[
    aiTree_ = actor_->AddComponent<BehaviorTree<BotEnemy>>();
    aiTree_->SetOwner(std::static_pointer_cast<BotEnemy>(shared_from_this()));

    //BehaviorTree���\�z
    aiTree_->AddNode("", "Root", 0, Ai_SelectRule::Priority,nullptr,nullptr);

    //�퓬
    aiTree_->AddNode("Root", "Battle", 0, Ai_SelectRule::Sequence, new BotBattleJudgment(this), nullptr);
    //��@
    aiTree_->AddNode("Root", "Scout",  1, Ai_SelectRule::Sequence, nullptr, nullptr);

    //�퓬�m�[�h
    aiTree_->AddNode("Battle", "Attack", 0, Ai_SelectRule::Non, new AttackJudgment(this), new BotAttackAction(this));
    aiTree_->AddNode("Battle", "Dodge",  1, Ai_SelectRule::Non, nullptr, new BotSideDodgeAction(this));
    //aiTree_->AddNode("Battle", "Dodge", 1, Ai_SelectRule::Non, new DodgeJudgment(this), new BotSideDodgeAction(this));

    //��@�m�[�h
    aiTree_->AddNode("Scout", "Wonder", 0, Ai_SelectRule::Non, nullptr, new BotWonderActioin(this));
    aiTree_->AddNode("Scout", "Idle",   1, Ai_SelectRule::Non, nullptr, new BotIdleAction(this));
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
    // ���G�͈͂��f�o�b�O�~���`��
    Engine::renderManager_->debugRenderer_->DrawCylinder(transform_->GetPosition(), searchAreaRadius_, 0.5f, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.7f));
#endif // _DEBUG
}

bool BotEnemy::SearchTarget()
{
    //�^�[�Q�b�g�擾
    if (!targetActor_)
    {
        targetActor_ = Engine::sceneManager_->GetActiveScene().Find("Player").lock();
        if (!targetActor_)return false;
    }

    //�^�[�Q�b�g�����G�͈͓��ɓ����Ă��邩
    const Vector3 myPos = transform_->GetPosition();
    const Vector3 targetPos = targetActor_->GetTransform()->GetPosition();
    Vector3 toTarget = targetPos - myPos;
    float dist = toTarget.Length();
    if (dist < searchAreaRadius_)
    {
        //���C�L���X�g�ŏ�Q�����^�[�Q�b�g�܂łɂȂ�������
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
            //���ϒl���王��p���l��
            toTarget.Normalize();
            float dot = toTarget.Dot(transform_->GetForward());

            if (dot > 1.0f - fov_)
            {
                //���E���ɓ����Ă���
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
    //�^�[�Q�b�g���w��͈͓��ɂ��邩����
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
    //�e���ʒu��ݒ�
    gunComponent_->SetMuzzlePos(transform_->GetPosition() + Vector3(0, 0.3f, 0));

    const Vector3 pos = transform_->GetPosition();
    Vector3 toAimPos = aimPosition_ - pos;
    toAimPos.Normalize();
    gunComponent_->Shot(toAimPos);

    //�U����s��
    canAttack_ = false;

    //�����[�h�t���O���I��
    reloadNow_ = true;
}

void BotEnemy::ReloadUpdate()
{
    //�����[�h���Ă��邩
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

const std::shared_ptr<AbyssEngine::Animator>& BotEnemy::GetAnimator() const
{
    return model_->GetAnimator();
}
