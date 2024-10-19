#include "BossMech.h"
#include "Actor.h"
#include "SkeletalMesh.h"
#include "GameCollider.h"
#include "BehaviorTree.h"
#include "ActionDerived.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Vitesse.h"
#include "AttackerSystem.h"

using namespace AbyssEngine;

#define Ai_SelectRule BehaviorTree<BossMech>::SelectRule

void BossMech::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    HumanoidWeapon::Initialize(actor);

    //パラメータの設定
    health_ = 1000.0f;
    Max_Health = 1000.0f;
    Max_Horizontal_Speed = 100.0f;
    Max_Vertical_Speed = 100.0f;
    baseRotSpeed_ = 500.0f;
    acceleration_ = 100.0f;
    deceleration_ = 60.0f;
    speedingDecel_ = 200.0f;
    Gravity = -30.0f;


    //enableGravity_ = false;

    //model_ = actor->AddComponent<AbyssEngine::SkeletalMesh>("./Assets/Models/Enemy/Boss/Mech_Idle.glb");
    model_ = actor->AddComponent<AbyssEngine::SkeletalMesh>("./Assets/Models/Enemy/Boss/Mech_Idle.gltf");
    model_->GetAnimator()->AppendAnimations({
        "./Assets/Models/Enemy/Boss/Mech_Run.glb",
        "./Assets/Models/Enemy/Boss/Mech_Jump_Start.glb",
        "./Assets/Models/Enemy/Boss/Mech_Jump_Loop.glb",
        "./Assets/Models/Enemy/Boss/Mech_Jump_End.glb",
        "./Assets/Models/Enemy/Boss/Mech_CrouchingStart.glb",
        },
        {
            "Run",
            "Jump_Start",
            "Jump_Loop",
            "Jump_End",
            "Crouching"
        }
    );

    model_->GetAnimator()->GetAnimations()[static_cast<float>(AnimationIndex::Crouching)]->SetLoopFlag(false);

    transform_->SetScaleFactor(35.0f);

   /* const auto& coll = AddHitCollider(Vector3::Zero, 10.0f, "Collider_Chest");
    coll->AttachModel(model_, "spine_02");*/

    //コライダー設定
    ColliderInitialize();

    //AI初期化
    BehaviorTreeInitialize();

    //アタッカーシステム
    AttackerSystemInitialize();

    //ターゲットになるヴィテスを取得
    const auto& vitesseActor = Engine::sceneManager_->GetActiveScene().Find("Vitesse");
    if (const auto& a = vitesseActor.lock())
    {
        targetVitesse_ = a->GetComponent<Vitesse>();
    }
}


void BossMech::RushAttackUpdate()
{
    moveVec_ = transform_->GetForward();
    if (const auto& target = targetVitesse_.lock())
    {
        Vector3 toTarget = target->GetTransform()->GetPosition() - transform_->GetPosition();
        toTarget.Normalize();
        moveVec_ = toTarget;
    }
}

void BossMech::ColliderInitialize()
{
    //喰らい判定設定
    {
        const std::shared_ptr<HitCollider> colliders[] =
        {
            AddHitCollider(Vector3::Zero, 10.0f, "Collider_Chest",          model_, "spine_02"),

            AddHitCollider(Vector3::Zero,  6.0f, "Collider_Head",           model_, "head"),

            AddHitCollider(Vector3::Zero,  6.0f, "Collider_Shoulder_R",     model_, "clavicle_r"),
            AddHitCollider(Vector3::Zero,  6.0f, "Collider_Upperarm_R",     model_, "upperarm_r"),
            AddHitCollider(Vector3::Zero,  6.0f, "Collider_Lowerarm_R",     model_, "lowerarm_r"),
            AddHitCollider(Vector3::Zero,  6.0f, "Collider_Hand_R",         model_, "hand_r"),

            AddHitCollider(Vector3::Zero,  6.0f, "Collider_Shoulder_L",     model_, "clavicle_l"),
            AddHitCollider(Vector3::Zero,  6.0f, "Collider_Upperarm_L",     model_, "upperarm_l"),
            AddHitCollider(Vector3::Zero,  6.0f, "Collider_Lowerarm_L",     model_, "lowerarm_l"),
            AddHitCollider(Vector3::Zero,  6.0f, "Collider_Hand_L",         model_, "hand_l"),

            AddHitCollider(Vector3::Zero, 6.0f, "Collider_Hip",            model_, "pelvis"),

            //大腿
            AddHitCollider(Vector3::Zero,  6.0f, "Collider_Thigh_R",         model_, "thigh_r"),
            AddHitCollider(Vector3(0.2f,0,0),  6.0f, "Collider_Lowthing_R",          model_, "thigh_r"),
            AddHitCollider(Vector3::Zero,  8.0f, "Collider_Knee_R",          model_, "calf_r"),
            AddHitCollider(Vector3(0.3f,0,0),  6.0f, "Collider_Downknee_R",          model_, "calf_r"),
            AddHitCollider(Vector3::Zero,  6.0f, "Collider_Foot_R",          model_, "foot_r"),

            AddHitCollider(Vector3::Zero,  6.0f, "Collider_Thigh_L",         model_, "thigh_l"),
            AddHitCollider(Vector3(-0.2f,0,0),  6.0f, "Collider_Lowthing_L",          model_, "thigh_l"),
            AddHitCollider(Vector3::Zero,  8.0f, "Collider_Knee_L",          model_, "calf_l"),
            AddHitCollider(Vector3(-0.3f,0,0),  6.0f, "Collider_Downknee_L",          model_, "calf_l"),
            AddHitCollider(Vector3::Zero,  6.0f, "Collider_Foot_L",          model_, "foot_l")
        };


        //ロックオン可能なコライダーを設定
        std::string lockonColliders[] = {
            "Collider_Chest",
            "Collider_Head",
            "Collider_Lowerarm_R",
            "Collider_Lowerarm_L",
            "Collider_Hip",
            "Collider_Knee_R",
            "Collider_Knee_L"
        };

        //タグを設定
        for (const auto& col : colliders)
        {
            col->ReplaceTag(Collider::Tag::Enemy);

            //ロックオン可能なコライダーを設定
            for (const auto& lockon : lockonColliders)
            {
                if (col->GetActor()->name_ == lockon)
                {
                    col->SetLockonTarget(true);
                }
            }
        }
    }

    //攻撃判定設定
    {
        const std::shared_ptr<AttackCollider> colliders[] =
        {
            AddAttackCollider(Vector3::Zero, 11.0f, "Collider_Chest",          model_, "spine_02"),

            AddAttackCollider(Vector3::Zero,  7.0f, "Collider_Head",           model_, "head"),

            AddAttackCollider(Vector3::Zero,  7.0f, "Collider_Shoulder_R",     model_, "clavicle_r"),
            AddAttackCollider(Vector3::Zero,  7.0f, "Collider_Upperarm_R",     model_, "upperarm_r"),
            AddAttackCollider(Vector3::Zero,  7.0f, "Collider_Lowerarm_R",     model_, "lowerarm_r"),
            AddAttackCollider(Vector3::Zero,  7.0f, "Collider_Hand_R",         model_, "hand_r"),

            AddAttackCollider(Vector3::Zero,  7.0f, "Collider_Shoulder_L",     model_, "clavicle_l"),
            AddAttackCollider(Vector3::Zero,  7.0f, "Collider_Upperarm_L",     model_, "upperarm_l"),
            AddAttackCollider(Vector3::Zero,  7.0f, "Collider_Lowerarm_L",     model_, "lowerarm_l"),
            AddAttackCollider(Vector3::Zero,  7.0f, "Collider_Hand_L",         model_, "hand_l"),

            AddAttackCollider(Vector3::Zero, 7.0f, "Collider_Hip",            model_, "pelvis"),

            //大腿
            AddAttackCollider(Vector3::Zero,  7.0f, "Collider_Thigh_R",         model_, "thigh_r"),
            AddAttackCollider(Vector3(0.2f,0,0),  7.0f, "Collider_Lowthing_R",          model_, "thigh_r"),
            AddAttackCollider(Vector3::Zero,  9.0f, "Collider_Knee_R",          model_, "calf_r"),
            AddAttackCollider(Vector3(0.3f,0,0),  7.0f, "Collider_Downknee_R",          model_, "calf_r"),
            AddAttackCollider(Vector3::Zero,  7.0f, "Collider_Foot_R",          model_, "foot_r"),

            AddAttackCollider(Vector3::Zero,  7.0f, "Collider_Thigh_L",         model_, "thigh_l"),
            AddAttackCollider(Vector3(-0.2f,0,0),  7.0f, "Collider_Lowthing_L",          model_, "thigh_l"),
            AddAttackCollider(Vector3::Zero,  9.0f, "Collider_Knee_L",          model_, "calf_l"),
            AddAttackCollider(Vector3(-0.3f,0,0),  7.0f, "Collider_Downknee_L",          model_, "calf_l"),
            AddAttackCollider(Vector3::Zero,  7.0f, "Collider_Foot_L",          model_, "foot_l")
        };

        //タグを設定
        for (const auto& col : colliders)
        {
            col->ReplaceTag(Collider::Tag::Enemy);
            attackColliders_.emplace_back(col);
        }
    }


    //押し出し判定設定
    {
        const std::shared_ptr<TerrainCollider> collider[] =
        {
            AddTerrainCollider(Vector3::Zero, 10.0f, "Collider_Chest",          model_, "spine_02"),

            AddTerrainCollider(Vector3::Zero,  6.0f, "Collider_Head",           model_, "head"),

            AddTerrainCollider(Vector3::Zero,  6.0f, "Collider_Shoulder_R",     model_, "clavicle_r"),
            AddTerrainCollider(Vector3::Zero,  6.0f, "Collider_Upperarm_R",     model_, "upperarm_r"),
            AddTerrainCollider(Vector3::Zero,  6.0f, "Collider_Lowerarm_R",     model_, "lowerarm_r"),
            AddTerrainCollider(Vector3::Zero,  6.0f, "Collider_Hand_R",         model_, "hand_r"),

            AddTerrainCollider(Vector3::Zero,  6.0f, "Collider_Shoulder_L",     model_, "clavicle_l"),
            AddTerrainCollider(Vector3::Zero,  6.0f, "Collider_Upperarm_L",     model_, "upperarm_l"),
            AddTerrainCollider(Vector3::Zero,  6.0f, "Collider_Lowerarm_L",     model_, "lowerarm_l"),
            AddTerrainCollider(Vector3::Zero,  6.0f, "Collider_Hand_L",         model_, "hand_l"),

            AddTerrainCollider(Vector3::Zero, 6.0f, "Collider_Hip",            model_, "pelvis"),

            //大腿
            AddTerrainCollider(Vector3::Zero,  6.0f, "Collider_Thigh_R",         model_, "thigh_r"),
            AddTerrainCollider(Vector3(0.2f,0,0),  6.0f, "Collider_Lowthing_R",          model_, "thigh_r"),
            AddTerrainCollider(Vector3::Zero,  8.0f, "Collider_Knee_R",          model_, "calf_r"),
            AddTerrainCollider(Vector3(0.3f,0,0),  6.0f, "Collider_Downknee_R",          model_, "calf_r"),
            AddTerrainCollider(Vector3::Zero,  6.0f, "Collider_Foot_R",          model_, "foot_r"),

            AddTerrainCollider(Vector3::Zero,  6.0f, "Collider_Thigh_L",         model_, "thigh_l"),
            AddTerrainCollider(Vector3(-0.2f,0,0),  6.0f, "Collider_Lowthing_L",          model_, "thigh_l"),
            AddTerrainCollider(Vector3::Zero,  8.0f, "Collider_Knee_L",          model_, "calf_l"),
            AddTerrainCollider(Vector3(-0.3f,0,0),  6.0f, "Collider_Downknee_L",          model_, "calf_l"),
            AddTerrainCollider(Vector3::Zero,  6.0f, "Collider_Foot_L",          model_, "foot_l")
        };

        //タグを設定
        for (const auto& col : collider)
        {
            col->ReplaceTag(Collider::Tag::Enemy);
        }
    }
}

void BossMech::BehaviorTreeInitialize()
{
    //ビヘイビアツリー
    aiTree_ = actor_->AddComponent<BehaviorTree<BossMech>>();
    aiTree_->SetOwner(std::static_pointer_cast<BossMech>(shared_from_this()));

    //BehaviorTreeを構築
    aiTree_->AddNode("", "Root", 0, Ai_SelectRule::Priority, nullptr, nullptr);

    //戦闘
    aiTree_->AddNode("Root", "Battle", 0, Ai_SelectRule::Sequence, new MechBattleJudgment(this), nullptr);
    //偵察
    aiTree_->AddNode("Root", "Scout", 1, Ai_SelectRule::Sequence, nullptr, nullptr);

    //戦闘ノード
    aiTree_->AddNode("Battle", "Attack", 0, Ai_SelectRule::Non, new MechRunAttackJudgment(this), new MechRunAttackAction(this));
    //aiTree_->AddNode("Battle", "Dodge", 1, Ai_SelectRule::Non, new DodgeJudgment(this), new BotSideDodgeAction(this));

    //偵察ノード
    aiTree_->AddNode("Scout", "Idle", 1, Ai_SelectRule::Non, nullptr, new MechIdleAction(this));

#if 1
    aiTree_->SetActive(false);
#endif // 1
}

void BossMech::AttackerSystemInitialize()
{
    attackerSystem_ = actor_->AddComponent<AttackerSystem>();

    AttackData atkData;
    for (auto& collider : attackColliders_)
    {
        atkData.attackColliderList_.emplace_back(collider);
    }
    atkData.power_ = 10.0f;
    atkData.duration_ = 7.0f;
    atkData.maxHits_ = 1.0f;
    atkData.staggerValue_ = 1.0f;
    atkData.hitStopDuration_ = 0.0f;
    atkData.hitStopOutTime_ = 0.0f;
    atkData.knockback_ = 400.0f;
    atkData.staggerType_ = StaggerType::Middle;

    attackerSystem_->RegistAttackData("Rush", atkData);
}
