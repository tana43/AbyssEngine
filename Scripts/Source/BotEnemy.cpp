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

    //����p��ݒ�
    fov_ = degreeFov_ / 360.0f;

    //�U���ݒ�
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
