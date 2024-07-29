#include "EnemyBot.h"
#include "Actor.h"
#include "SkeletalMesh.h"

using namespace AbyssEngine;

void EnemyBot::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //初期化
    BaseEnemy::Initialize(actor);

    //スケルタルメッシュを追加
    model_ = actor->AddComponent<SkeletalMesh>("./Assets/Models/Enemy/Bot_Rolling.glb");
    /*model_->GetAnimator()->AppendAnimations(
        {
        {
        "./Assets/Models/Enemy/Bot_Idle.glb",
        "./Assets/Models/Enemy/Bot_Walk.glb",
        "./Assets/Models/Enemy/Bot_Jump.glb",
        "./Assets/Models/Enemy/Bot_Search.glb",
        "./Assets/Models/Enemy/Bot_Attack.glb",
        "./Assets/Models/Enemy/Bot_Attack_Assult.glb"
        },
        {
            "Rolling","Walk","Jump","Search","Attack","Attack_Assult"
        }
        });*/
}
