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
#include "Gun.h"
#include "Input.h"
#include "BillboardRenderer.h"

using namespace AbyssEngine;

#define Ai_SelectRule BehaviorTree<BossMech>::SelectRule

void BossMech::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    HumanoidWeapon::Initialize(actor);

    //パラメータの設定
    //health_ = 1000.0f;
    health_ = 10.0f;
    Max_Health = 1000.0f;
    Max_Horizontal_Speed = 200.0f;
    Max_Vertical_Speed = 200.0f;
    baseRotSpeed_ = 3000.0f;
    Max_Rot_Speed = 3000.0f;
    acceleration_ = 1000.0f;
    deceleration_ = 60.0f;
    speedingDecel_ = 200.0f;
    Gravity = -30.0f;
    center_ = { 0,100,0 };

    isLimitSpeed_ = true;


    //enableGravity_ = false;

    //model_ = actor->AddComponent<AbyssEngine::SkeletalMesh>("./Assets/Models/Enemy/Boss/Mech_Idle.glb");
    model_ = actor->AddComponent<AbyssEngine::SkeletalMesh>("./Assets/Models/Enemy/Boss/Mech_Idle.gltf");
    model_->GetAnimator()->AppendAnimations({
        "./Assets/Models/Enemy/Boss/Animations/Mech_Run.gltf",
        "./Assets/Models/Enemy/Boss/Animations/Mech_Jump_Start.gltf",
        "./Assets/Models/Enemy/Boss/Animations/Mech_Jump_Loop.gltf",
        "./Assets/Models/Enemy/Boss/Animations/Mech_Jump_End.gltf",
        "./Assets/Models/Enemy/Boss/Animations/Mech_CrouchingStart.gltf",

        "./Assets/Models/Enemy/Boss/Animations/Mech_Fly_Idle.gltf",
        "./Assets/Models/Enemy/Boss/Animations/Mech_Skill_03_Attacker.gltf",

        //"./Assets/Models/Enemy/Boss/Animations/Mech_Skill_03_Attacker.gltf",
        //"./Assets/Models/Enemy/Boss/Animations/Mech_Skill_03_Attacker.gltf",
        //"./Assets/Models/Enemy/Boss/Animations/Mech_Skill_03_Attacker.gltf",
         
         
        "./Assets/Models/Enemy/Boss/Animations/Mech_Combo_01.gltf",
        "./Assets/Models/Enemy/Boss/Animations/Mech_Combo_02.gltf",
        "./Assets/Models/Enemy/Boss/Animations/Mech_Combo_03.gltf",

        //"./Assets/Models/Enemy/Boss/Animations/Mech_Dash_Dodge_Left.gltf",
        //"./Assets/Models/Enemy/Boss/Animations/Mech_Dash_Dodge_Right.gltf",
        //"./Assets/Models/Enemy/Boss/Animations/Mech_Dash_Forward.gltf",
        //"./Assets/Models/Enemy/Boss/Animations/Mech_Fly_Front_End.gltf",

        "./Assets/Models/Enemy/Boss/Animations/Mech_Fly_Front_Start.gltf",
        "./Assets/Models/Enemy/Boss/Animations/Mech_Fly_Front_Loop.gltf",
        "./Assets/Models/Enemy/Boss/Animations/Mech_Fly_Front_End.gltf",



        },
        {
            "Run",
            "Jump_Start",
            "Jump_Loop",
            "Jump_End",
            "Crouching",
            "Fly_Idle",
            "Skill_01",
            "Combo_01",
            "Combo_02",
            "Combo_03",
            "Fly_Front_Start",
            "Fly_Front_Loop",
            "Fly_Front_End",
        }
    );

    model_->GetAnimator()->GetAnimations()[static_cast<float>(AnimationIndex::Crouching)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<float>(AnimationIndex::Skill_01)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<float>(AnimationIndex::Fly_Front_Start)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<float>(AnimationIndex::Fly_Front_Start)]->SetAnimSpeed(2.0f);
    model_->GetAnimator()->GetAnimations()[static_cast<float>(AnimationIndex::Fly_Front_End)]->SetLoopFlag(false);

    model_->GetAnimator()->GetAnimations()[static_cast<float>(AnimationIndex::Combo_01)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<float>(AnimationIndex::Combo_02)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<float>(AnimationIndex::Combo_03)]->SetLoopFlag(false);

    transform_->SetScaleFactor(35.0f);

   /* const auto& coll = AddHitCollider(Vector3::Zero, 10.0f, "Collider_Chest");
    coll->AttachModel(model_, "spine_02");*/

    //コライダー設定
    ColliderInitialize();

    //AI初期化
    BehaviorTreeInitialize();

    //アタッカーシステム
    AttackerSystemInitialize();


    gunComL_ = actor->AddComponent<Gun>();
    gunComL_->SetIsHoming(true);
    gunComL_->SetTargetTag(Actor::Tag_Player);
    gunComL_->SetBulletType(Gun::BulletType::Beam);
    gunComL_->SetBeamColor(Vector4(0.80f, 0.15f, 0.0f, 1.0f));
    gunComL_->SetBeamParticleColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    gunComL_->SetBeamIntensity(1.5f);
    gunComL_->SetBeamParticleIntensity(30.0f);
    gunComL_->SetEnableMuzzleFlashParticleEffect(false);
    gunComL_->SetBulletSpeed(200.0f);
    gunComL_->SetHomingStrength(2.0f);
    gunComL_->SetColliderTag(Collider::Tag::Enemy);
    gunComL_->SetActiveRateOfFire(true);
    gunComL_->SetBulletLifespan(5.0f);

    gunComL_->SetRateOfFire(0.1f);

    gunComL_->GetBeamMuzzleFlashComponent()->SetColor(Vector4(1.0f, 0.2f, 0.0f, 1.0f));
    gunComL_->GetBeamMuzzleFlashComponent()->SetIntensity(10.0f);

    actor->ReplaceTag(Actor::Tag_Enemy);

    //ターゲットになるヴィテスを取得
    const auto& vitesseActor = Engine::sceneManager_->GetActiveScene().Find("Vitesse");
    if (const auto& a = vitesseActor.lock())
    {
        targetVitesse_ = a->GetComponent<Vitesse>();

        if (const auto& center = a->FindChild("HitCollider_LowChest").lock())
        {
            gunComL_->SetTargetTransform(center->GetTransform());
        }
    }
}

void BossMech::Update()
{
    HumanoidWeapon::Update();

    UpdateMuzzlePos();
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

bool BossMech::ShotHomingBeam()
{
#if 0
    float interval = DirectX::XM_2PI / static_cast<float>(shotHomingBeamCount_);
    for (int i = 0; i < shotHomingBeamCount_; i++)
    {
        //float sin = sinf(shotDireTimer_);
        float x = interval * i;
        float sin = sinf(x);
        Vector3 forward = transform_->GetForward();
        Vector3 right = transform_->GetRight();
        //仮でビーム攻撃
        Vector3 dire = {
            sin * forward.x + sin * right.x,
            cosf(x),
            sin * forward.z + sin * right.z
        };
        dire.Normalize();
        return gunComL_->Shot(dire);
        //shotDireTimer_ += Time::GetDeltaTime() * 5.0f;

    }
#else
    return gunComL_->Shot(transform_->GetForward());
#endif // 0
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

        //中心になるコライダー設定
        coreCollider_ = collider[0]->GetActor();
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
    {
        //ヴィテスまで接近
        aiTree_->AddNode("Battle", "MoveToVitesse", 0, Ai_SelectRule::Non, nullptr, new MechMoveToVitesseAction(this));
        //aiTree_->AddNode("Battle", "Combo_01", 0, Ai_SelectRule::Non, nullptr, new MechCombo01Action(this));
        //aiTree_->AddNode("Battle", "Combo_02", 0, Ai_SelectRule::Non, nullptr, new MechCombo02Action(this));
        //aiTree_->AddNode("Battle", "Combo_03", 0, Ai_SelectRule::Non, nullptr, new MechCombo03Action(this));
        // 
        //３種のコンボをどれか自動で選択し、実行させる
        aiTree_->AddNode("Battle", "Combo_Random", 0, Ai_SelectRule::Random, nullptr, nullptr);
        {
            aiTree_->AddNode("Combo_Random", "Combo_01", 0, Ai_SelectRule::Non, nullptr, new MechCombo01Action(this));
            aiTree_->AddNode("Combo_Random", "Combo_02", 0, Ai_SelectRule::Non, nullptr, new MechCombo02Action(this));
            aiTree_->AddNode("Combo_Random", "Combo_03", 0, Ai_SelectRule::Non, nullptr, new MechCombo03Action(this));

        }

        aiTree_->AddNode("Battle", "FlyIdle", 0, Ai_SelectRule::Non, nullptr, new MechFlyIdleAction(this));


        //aiTree_->AddNode("Battle", "ShotBeam", 0, Ai_SelectRule::Non, new MechShotBeamJudgment(this), new MechShotBeamAction(this));
        //aiTree_->AddNode("Battle", "Attack", 0, Ai_SelectRule::Non, new MechRunAttackJudgment(this), new MechRunAttackAction(this));
        //aiTree_->AddNode("Battle", "Dodge", 1, Ai_SelectRule::Non, new DodgeJudgment(this), new BotSideDodgeAction(this));
    }

    //偵察ノード
    aiTree_->AddNode("Scout", "Idle", 1, Ai_SelectRule::Non, new MechGroundJudgment(this), new MechIdleAction(this));
    aiTree_->AddNode("Scout", "FlyIdle", 1, Ai_SelectRule::Non, new MechFlyJudgment(this), new MechFlyIdleAction(this));

#if 1
    aiTree_->SetActive(false);
#endif // 1
}

void BossMech::AttackerSystemInitialize()
{
    attackerSystem_ = actor_->AddComponent<AttackerSystem>();

    //突進攻撃
    {
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

    //コンボ攻撃01
    {
        AttackData atkData;
        for (auto& collider : attackColliders_)
        {
            //登録するコライダー名
            if (
                collider->GetActor()->name_ == "Collider_Lowerarm_L" ||
                collider->GetActor()->name_ == "Collider_Hand_L" ||
                collider->GetActor()->name_ == "Collider_Thigh_R"||
                collider->GetActor()->name_ == "Collider_Knee_R" ||
                collider->GetActor()->name_ == "Collider_Downknee_R" ||
                collider->GetActor()->name_ == "Collider_Foot_R"
                )

            {
                atkData.attackColliderList_.emplace_back(collider);
            }
        }
        atkData.power_ = 10.0f;
        atkData.duration_ = 1.0f;
        atkData.maxHits_ = 1.0f;
        atkData.staggerValue_ = 1.0f;
        atkData.hitStopDuration_ = 0.0f;
        atkData.hitStopOutTime_ = 0.0f;
        atkData.knockback_ = 100.0f;
        atkData.isHitRotate_ = true;//攻撃がヒットした時にすぐにその方向へ相手を回転させる
        atkData.staggerType_ = StaggerType::Middle;

        attackerSystem_->RegistAttackData("Combo_01", atkData);
    }

    //コンボ攻撃02
    {
        AttackData atkData;
        for (auto& collider : attackColliders_)
        {
            //登録するコライダー名
            if (
                collider->GetActor()->name_ == "Collider_Lowerarm_R" ||
                collider->GetActor()->name_ == "Collider_Hand_R" ||
                collider->GetActor()->name_ == "Collider_Lowerarm_L" ||
                collider->GetActor()->name_ == "Collider_Hand_L"
                )
            {
                atkData.attackColliderList_.emplace_back(collider);
            }
        }
        atkData.power_ = 10.0f;
        atkData.duration_ = 1.0f;
        atkData.maxHits_ = 1.0f;
        atkData.staggerValue_ = 1.0f;
        atkData.hitStopDuration_ = 0.0f;
        atkData.hitStopOutTime_ = 0.0f;
        atkData.knockback_ = 200.0f;
        atkData.isHitRotate_ = true;//攻撃がヒットした時にすぐにその方向へ相手を回転させる
        atkData.staggerType_ = StaggerType::Middle;

        attackerSystem_->RegistAttackData("Combo_02", atkData);
    }

    //コンボ攻撃03
    {
        AttackData atkData;
        for (auto& collider : attackColliders_)
        {
            //登録するコライダー名
            if (
                collider->GetActor()->name_ == "Collider_Lowerarm_R" ||
                collider->GetActor()->name_ == "Collider_Hand_R" ||
                collider->GetActor()->name_ == "Collider_Thigh_L" ||
                collider->GetActor()->name_ == "Collider_Knee_L" ||
                collider->GetActor()->name_ == "Collider_Downknee_L" ||
                collider->GetActor()->name_ == "Collider_Foot_L"
                )
            {
                atkData.attackColliderList_.emplace_back(collider);
            }
        }
        atkData.power_ = 10.0f;
        atkData.duration_ = 1.0f;
        atkData.maxHits_ = 1.0f;
        atkData.staggerValue_ = 1.0f;
        atkData.hitStopDuration_ = 0.0f;
        atkData.hitStopOutTime_ = 0.0f;
        atkData.knockback_ = 100.0f;
        atkData.isHitRotate_ = true;//攻撃がヒットした時にすぐにその方向へ相手を回転させる
        atkData.staggerType_ = StaggerType::Middle;

        attackerSystem_->RegistAttackData("Combo_03", atkData);
    }

}

void BossMech::UpdateMuzzlePos()
{
    Matrix mat = model_->FindSocket("hand_r");
    mat = mat * transform_->GetWorldMatrix();
    Vector3 muzzlePos = { mat.m[3][0],mat.m[3][1],mat.m[3][2] };
    gunComL_->SetMuzzlePos(muzzlePos);

    if (Keyboard::instance_->GetKeyDown(DirectX::Keyboard::X))
    {
        ShotHomingBeam();
    }
}

bool BossMech::MoveTo(Vector3 goalPos)
{
    Vector3 vec = goalPos - coreCollider_.lock()->GetTransform()->GetPosition();
    Vector3 vecNormal;
    vec.Normalize(vecNormal);

    //キャラを動かす
    velocity_ = vecNormal * Max_Horizontal_Speed;

    const float naerRange = 40.0f;

    if (vec.LengthSquared() < naerRange * naerRange)
    {
        return true;
    }

    return false;
}
