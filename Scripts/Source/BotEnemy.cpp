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

#include "imgui/imgui.h"


using namespace AbyssEngine;

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

    //�p�����[�^�̐ݒ�
    transform_->SetScaleFactor(0.2f);
    Max_Horizontal_Speed = 2.0f;
    acceleration_ = 0.5f;

    reloadTimer_ = 0.0f;

    //����p��ݒ�
    fov_ = degreeFov_ / 360.0f;

    //�U���ݒ�
    canAttack_ = true;

    //�e
    gunComponent_ = AddComponent<Gun>();

    //�r�w�C�r�A�c���[������
    BehaviorTreeInitialize();
}

void BotEnemy::BehaviorTreeInitialize()
{
    //�r�w�C�r�A�c���[
    behaviorTree_ = AddComponent<AbyssEngine::BehaviorTree<BotEnemy>>();
    behaviorTree_->
}

void BotEnemy::Update()
{
    BaseEnemy::Update();
}

bool BotEnemy::DrawImGui()
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

    return false;
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
    }

    //�^�[�Q�b�g�����G�͈͓��ɓ����Ă��邩
    const Vector3 myPos = transform_->GetPosition();
    const Vector3 targetPos = targetActor_->GetTransform()->GetPosition();
    Vector3 toTarget = targetPos - myPos;
    float dist = toTarget.Length();
    if (dist < searchAreaRadius_)
    {
        //���C�L���X�g�Ŏ��E�Ƀ^�[�Q�b�g�������Ă��邩����
        Vector3 hitPos,hitNormal;
        auto& stage = StageManager::Instance().GetActiveStage();
        if (stage->RayCast(
            myPos,
            targetPos,
            hitPos,
            hitNormal
        ))
        {
            //���ϒl���王��p���l��
            toTarget.Normalize();
            float dot = toTarget.Dot(transform_->GetForward());

            if (dot > 1.0f - fov_)
            {
                //���E���ɓ����Ă���
                return true;
            }
        }
    }

    return false;
}

void BotEnemy::LockOn()
{
    aimPosition_ = targetActor_->GetTransform()->GetPosition();
}

void BotEnemy::Shot()
{
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

    reloadTimer_ += Time::deltaTime_;

    if (reloadTimer_ > Atk_Reload_Time)
    {
        reloadNow_ = false;
        canAttack_ = true;
        reloadTimer_ = 0.0f;
    }
}

const std::shared_ptr<AbyssEngine::Animator>& BotEnemy::GetAnimator() const
{
    return model_->GetAnimator();
}
