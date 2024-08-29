#include "EnemyBot.h"
#include "Actor.h"
#include "SkeletalMesh.h"

using namespace AbyssEngine;

void EnemyBot::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
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
    {
        transform_->SetScaleFactor(0.2f);
        Max_Horizontal_Speed = 2.0f;
        acceleration_ = 0.5f;
    }
}

void EnemyBot::Update()
{
    BaseEnemy::Update();
}
