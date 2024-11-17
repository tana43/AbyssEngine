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
#include "PlayerSoldier.h"

using namespace AbyssEngine;

#define Ai_SelectRule BehaviorTree<BossMech>::SelectRule

void BossMech::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    HumanoidWeapon::Initialize(actor);

    //パラメータの設定
    health_ = 400.0f;
    //health_ = 10.0f;
    Max_Health = 400.0f;
    Max_Horizontal_Speed = 130.0f;
    Max_Vertical_Speed = 90.0f;
    baseRotSpeed_ = 1000.0f;
    Max_Rot_Speed = 1000.0f;
    acceleration_ = 300.0f;
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

        "./Assets/Models/Enemy/Boss/Animations/Mech_Fly_Right_Loop.gltf",
        "./Assets/Models/Enemy/Boss/Animations/Mech_Fly_Left_Loop.gltf",

        "./Assets/Models/Enemy/Boss/Animations/Mech_Fly_Back_Loop.gltf",



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
            "Fly_Left_Loop",
            "Fly_Right_Loop",
            "Fly_Back_Loop",
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


    model_->GetModel()->primitiveConstants_->data_.minAmbient = 0.4f;

   /* const auto& coll = AddHitCollider(Vector3::Zero, 10.0f, "Collider_Chest");
    coll->AttachModel(model_, "spine_02");*/

    //コライダー設定
    ColliderInitialize();

    //AI初期化
    BehaviorTreeInitialize();

    //アタッカーシステム
    AttackerSystemInitialize();


    //通常弾の設定
    {
        noramlHomingGunL_ = actor->AddComponent<Gun>();
        noramlHomingGunL_->SetIsHoming(true);
        noramlHomingGunL_->SetTargetTag(Actor::Tag_Player);
        noramlHomingGunL_->SetBulletType(Gun::BulletType::Beam);
        noramlHomingGunL_->SetBeamColor(Vector4(0.80f, 0.15f, 0.0f, 1.0f));
        noramlHomingGunL_->SetBeamParticleColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
        noramlHomingGunL_->SetBeamIntensity(4.0f);
        noramlHomingGunL_->SetBeamParticleIntensity(30.0f);
        noramlHomingGunL_->SetEnableMuzzleFlashParticleEffect(false);
        noramlHomingGunL_->SetBulletSpeed(200.0f);
        noramlHomingGunL_->SetHomingStrength(2.0f);
        noramlHomingGunL_->SetColliderTag(Collider::Tag::Enemy);
        noramlHomingGunL_->SetActiveRateOfFire(true);
        noramlHomingGunL_->SetBulletLifespan(5.0f);

        noramlHomingGunL_->SetRateOfFire(0.1f);

        noramlHomingGunL_->GetBeamMuzzleFlashComponent()->SetColor(Vector4(1.0f, 0.2f, 0.0f, 1.0f));
        noramlHomingGunL_->GetBeamMuzzleFlashComponent()->SetIntensity(10.0f);

        noramlHomingGunL_->SetHitParticleParam(ComputeParticleEmitter::GetJsonEmitParamater("Beam_Hit"));
        noramlHomingGunL_->SetHitFireParticleParam(ComputeParticleEmitter::GetJsonEmitParamater("BossMech_Beam_Hit_Fire"));
        noramlHomingGunL_->SetTerrainHitParticleParam(ComputeParticleEmitter::GetJsonEmitParamater("BossMech_Beam_Hit_Fire"));
    }

    //強ホーミング弾の設定
    {
        superHomingGunL_ = actor->AddComponent<Gun>();
        superHomingGunL_->SetIsHoming(true);
        superHomingGunL_->SetTargetTag(Actor::Tag_Player);
        superHomingGunL_->SetBulletType(Gun::BulletType::Beam);
        superHomingGunL_->SetBeamColor(Vector4(0.1f, 1.0f, 0.0f, 1.0f));
        superHomingGunL_->SetBeamParticleColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
        superHomingGunL_->SetBeamIntensity(4.5f);
        superHomingGunL_->SetBeamParticleIntensity(30.0f);
        superHomingGunL_->SetEnableMuzzleFlashParticleEffect(false);
        superHomingGunL_->SetBulletSpeed(250.0f);
        superHomingGunL_->SetHomingStrength(7.8f);
        superHomingGunL_->SetColliderTag(Collider::Tag::Enemy);
        superHomingGunL_->SetActiveRateOfFire(true);
        superHomingGunL_->SetBulletLifespan(5.0f);
        superHomingGunL_->SetBeamWidth(0.2f);
        superHomingGunL_->SetBeamScale(0.4f);

        superHomingGunL_->SetRateOfFire(0.1f);

        superHomingGunL_->GetBeamMuzzleFlashComponent()->SetColor(Vector4(0.1f, 1.0f, 0.0f, 1.0f));
        superHomingGunL_->GetBeamMuzzleFlashComponent()->SetIntensity(10.0f);

        superHomingGunL_->SetHitParticleParam(ComputeParticleEmitter::GetJsonEmitParamater("Beam_Hit"));
        superHomingGunL_->SetHitFireParticleParam(ComputeParticleEmitter::GetJsonEmitParamater("BossMech_Beam_Hit_Fire"));
    }

    //ミサイルの設定
    {
        missileGunL_ = actor->AddComponent<Gun>();
        missileGunL_->SetIsHoming(true);
        missileGunL_->SetTargetTag(Actor::Tag_Player);
        missileGunL_->SetBulletType(Gun::BulletType::Missile);
        missileGunL_->SetMissileColor(Vector4(1.0f, 0.5f, 0.0f, 1.0f));
        missileGunL_->SetMissileParticleColor(Vector4(0.2f, 0.0f, 1.0f, 1.0f));
        missileGunL_->SetMissileIntensity(1.88f);
        missileGunL_->SetMissileWidth(0.1f);
        missileGunL_->SetMissileScale(5.5f);
        missileGunL_->SetMissileParticleIntensity(0.220f);
        missileGunL_->SetEnableMuzzleFlashParticleEffect(false);
        missileGunL_->SetBulletSpeed(120.0f);
        missileGunL_->SetHomingStrength(2.0f);
        missileGunL_->SetColliderTag(Collider::Tag::Enemy);
        missileGunL_->SetActiveRateOfFire(true);
        missileGunL_->SetBulletLifespan(5.0f);

        missileGunL_->SetRateOfFire(0.1f);

        missileGunL_->GetBeamMuzzleFlashComponent()->SetColor(Vector4(1.0f, 0.2f, 0.0f, 1.0f));
        missileGunL_->GetBeamMuzzleFlashComponent()->SetIntensity(10.0f);

        missileGunL_->SetHitParticleParam(ComputeParticleEmitter::GetJsonEmitParamater("Beam_Hit"));
        missileGunL_->SetHitFireParticleParam(ComputeParticleEmitter::GetJsonEmitParamater("BossMech_Beam_Hit_Fire"));

        //ターゲットになるヴィテスを取得
        const auto& vitesseActor = Engine::sceneManager_->GetActiveScene().Find("Vitesse");
        if (const auto& a = vitesseActor.lock())
        {
            targetVitesse_ = a->GetComponent<Vitesse>();

            if (const auto& center = a->FindChild("HitCollider_LowChest").lock())
            {
                noramlHomingGunL_->SetTargetTransform(center->GetTransform());
                superHomingGunL_->SetTargetTransform(center->GetTransform());
                missileGunL_->SetTargetTransform(center->GetTransform());
            }
        }
    }



    actor->ReplaceTag(Actor::Tag_Enemy);

    
}

void BossMech::Update()
{
    HumanoidWeapon::Update();

    UpdateMuzzlePos();

#if _DEBUG
#else//リリース時のみ
    //ヴィテスにパイロットが搭乗したらAI起動
    if (!aiTree_->GetActive())
    {
        if (const auto& v = targetVitesse_.lock())
        {
            //パイロットが搭乗しているか
            if (const auto p = v->GetPilot().lock())
            {
                if (p->GetVitesseOnBoard())
                {
                    aiTree_->SetActive(true);
                }
            }
        }
    }
#endif
    

#if _DEBUG


    if (Keyboard::instance_->GetKeyDown(DirectX::Keyboard::X))
    {
        ShotNormalHomingBeam();
    }

    if (Keyboard::instance_->GetKeyDown(DirectX::Keyboard::C))
    {
        ShotSuperHomingBeam();
    }

    //拡散弾
    if (Keyboard::instance_->GetKeyDown(DirectX::Keyboard::V))
    {
        DiffusionShotSuperHomingBeam();
    }
    
    //ミサイル
    if (Keyboard::instance_->GetKeyDown(DirectX::Keyboard::M))
    {
        ShotMissile();
    }

#endif // _DEBUG
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

bool BossMech::ShotNormalHomingBeam()
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
    return noramlHomingGunL_->Shot(transform_->GetForward(),nullptr);
#endif // 0
}

bool BossMech::ShotSuperHomingBeam()
{
    return superHomingGunL_->Shot(transform_->GetForward(), nullptr);
}

bool BossMech::DiffusionShotSuperHomingBeam()
{
    //１フレームに何度でも連射できるように発射レートをオフにする
    superHomingGunL_->SetActiveRateOfFire(false);
    float interval = DirectX::XM_2PI / static_cast<float>(shotHomingBeamCount_);

    bool isShot = false;

    for (int i = 0; i < shotHomingBeamCount_; i++)
    {
        //float sin = sinf(shotDireTimer_);
        float x = interval * i;
        float sin = sinf(x);
        Vector3 up = transform_->GetUp();
        Vector3 right = transform_->GetRight();
        //仮でビーム攻撃
        Vector3 dire = {
            sin * up.x + sin * right.x,
            cosf(x),
            sin * up.z + sin * right.z
        };
        dire.Normalize();
        if (superHomingGunL_->Shot(dire))
        {
            isShot = true;
        }
        //shotDireTimer_ += Time::GetDeltaTime() * 5.0f;
    }

    //発射レートを復活させる
    superHomingGunL_->SetActiveRateOfFire(true);

    return isShot;
}

bool BossMech::ShotMissile()
{
    return missileGunL_->Shot(transform_->GetForward(), nullptr);
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


#if 1 //リリース用

    //死亡
    aiTree_->AddNode("Root", "Die", -1, Ai_SelectRule::Non, new MechDieJudgment(this), new MechDieAction(this));


    //戦闘
    aiTree_->AddNode("Root", "Battle", 0, Ai_SelectRule::Sequence, new MechBattleJudgment(this), nullptr);
    {
        //ヴィテスから離れる
        aiTree_->AddNode("Battle", "Back", 0, Ai_SelectRule::Non, new MechNearestRangeJudgment(this), new MechBackToVitesseAction(this));

        aiTree_->AddNode("Battle", "Attack", 1, Ai_SelectRule::Random, nullptr, nullptr);
        {
            //近接攻撃
            aiTree_->AddNode("Attack", "Melee", 0, Ai_SelectRule::Sequence, nullptr, nullptr);
            {
                aiTree_->AddNode("Melee", "MoveToVitesse", 0, Ai_SelectRule::Non, nullptr, new MechMoveToVitesseAction(this));

                aiTree_->AddNode("Melee", "Combo", 0, Ai_SelectRule::Random, nullptr, nullptr);
                {
                    aiTree_->AddNode("Combo", "Combo_01", 0, Ai_SelectRule::Non, nullptr, new MechCombo01Action(this));
                    aiTree_->AddNode("Combo", "Combo_02", 0, Ai_SelectRule::Non, nullptr, new MechCombo02Action(this));
                    aiTree_->AddNode("Combo", "Combo_03", 0, Ai_SelectRule::Non, nullptr, new MechCombo03Action(this));
                }
            }

            //射撃
            //中距離か判定
            aiTree_->AddNode("Attack", "Shot", 0, Ai_SelectRule::Sequence, new MechMiddleRangeJudgment(this),nullptr);
            {
                aiTree_->AddNode("Shot", "TurnToVitesse", 0, Ai_SelectRule::Non, nullptr, new MechTurnToVitesse(this));

                aiTree_->AddNode("Shot", "ShotBeam", 0, Ai_SelectRule::Random, nullptr, nullptr);
                {
                    aiTree_->AddNode("ShotBeam", "ShotNormalBeam", 0, Ai_SelectRule::Non, new MechShotBeamJudgment(this), new MechShotNormalBeamAction(this));
                    aiTree_->AddNode("ShotBeam", "ShotSuperHomingBeam", 0, Ai_SelectRule::Non, new MechShotBeamJudgment(this), new MechShotSuperHomingBeamAction(this));
                    aiTree_->AddNode("ShotBeam", "ShotMissile", 0, Ai_SelectRule::Non, new MechShotBeamJudgment(this), new MechShotMissileAction(this));
                }
                //隙を作っとく
                aiTree_->AddNode("Shot", "FlyIdle", 0, Ai_SelectRule::Non, nullptr, new MechFlyIdleAction(this));
                aiTree_->AddNode("Shot", "FlyIdle", 0, Ai_SelectRule::Non, nullptr, new MechFlyIdleAction(this));
            }


            aiTree_->AddNode("Attack", "SideMove", 0, Ai_SelectRule::Non, nullptr, new MechSideMoveAction(this));


            //aiTree_->AddNode("Attack", "FlyIdle", 0, Ai_SelectRule::Non, nullptr, new MechFlyIdleAction(this));

        }
    }

    //偵察
    aiTree_->AddNode("Root", "Scout", 1, Ai_SelectRule::Sequence, nullptr, nullptr);
    {
        //偵察ノード
        aiTree_->AddNode("Scout", "Idle", 1, Ai_SelectRule::Non, new MechGroundJudgment(this), new MechIdleAction(this));
        aiTree_->AddNode("Scout", "FlyIdle", 1, Ai_SelectRule::Non, new MechFlyJudgment(this), new MechFlyIdleAction(this));
    }


    


#else// デバッグ用
    //戦闘
    aiTree_->AddNode("Root", "Battle", 0, Ai_SelectRule::Sequence, new MechBattleJudgment(this), nullptr);
    //偵察
    aiTree_->AddNode("Root", "Scout", 1, Ai_SelectRule::Sequence, nullptr, nullptr);
    //戦闘
    aiTree_->AddNode("Root", "Die", -1, Ai_SelectRule::Non, new MechDieJudgment(this), new MechDieAction(this));
    //戦闘ノード
    {
        //ヴィテスまで接近
        //aiTree_->AddNode("Battle", "MoveToVitesse", 0, Ai_SelectRule::Non, nullptr, new MechMoveToVitesseAction(this));
        //aiTree_->AddNode("Battle", "Combo_01", 0, Ai_SelectRule::Non, nullptr, new MechCombo01Action(this));
        //aiTree_->AddNode("Battle", "Combo_02", 0, Ai_SelectRule::Non, nullptr, new MechCombo02Action(this));
        //aiTree_->AddNode("Battle", "Combo_03", 0, Ai_SelectRule::Non, nullptr, new MechCombo03Action(this));
        // 
        //３種のコンボをどれか自動で選択し、実行させる
        //aiTree_->AddNode("Battle", "Combo_Random", 0, Ai_SelectRule::Random, nullptr, nullptr);
        //{
        //    aiTree_->AddNode("Combo_Random", "Combo_01", 0, Ai_SelectRule::Non, nullptr, new MechCombo01Action(this));
        //    aiTree_->AddNode("Combo_Random", "Combo_02", 0, Ai_SelectRule::Non, nullptr, new MechCombo02Action(this));
        //    aiTree_->AddNode("Combo_Random", "Combo_03", 0, Ai_SelectRule::Non, nullptr, new MechCombo03Action(this));
        //}

        aiTree_->AddNode("Battle", "ShotBeam", 0, Ai_SelectRule::Non, new MechShotBeamJudgment(this), new MechShotNormalBeamAction(this));
        aiTree_->AddNode("Battle", "FlyIdle", 0, Ai_SelectRule::Non, nullptr, new MechFlyIdleAction(this));
        aiTree_->AddNode("Battle", "ShotSuperHomingBeam", 0, Ai_SelectRule::Non, new MechShotBeamJudgment(this), new MechShotSuperHomingBeamAction(this));
        aiTree_->AddNode("Battle", "FlyIdle", 0, Ai_SelectRule::Non, nullptr, new MechFlyIdleAction(this));
        aiTree_->AddNode("Battle", "ShotMissile", 0, Ai_SelectRule::Non, new MechShotBeamJudgment(this), new MechShotMissileAction(this));

        aiTree_->AddNode("Battle", "FlyIdle", 0, Ai_SelectRule::Non, nullptr, new MechFlyIdleAction(this));


        //aiTree_->AddNode("Battle", "Attack", 0, Ai_SelectRule::Non, new MechRunAttackJudgment(this), new MechRunAttackAction(this));
        //aiTree_->AddNode("Battle", "Dodge", 1, Ai_SelectRule::Non, new DodgeJudgment(this), new BotSideDodgeAction(this));
    }

    //偵察ノード
    aiTree_->AddNode("Scout", "Idle", 1, Ai_SelectRule::Non, new MechGroundJudgment(this), new MechIdleAction(this));
    aiTree_->AddNode("Scout", "FlyIdle", 1, Ai_SelectRule::Non, new MechFlyJudgment(this), new MechFlyIdleAction(this));

#endif // 0

    
#if _DEBUG
    aiTree_->SetActive(false);
#endif // _DEBUG
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
        atkData.knockback_ = 300.0f;
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
        atkData.knockback_ = 400.0f;
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
        atkData.knockback_ = 400.0f;
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
    noramlHomingGunL_->SetMuzzlePos(muzzlePos);
    superHomingGunL_->SetMuzzlePos(muzzlePos);
    missileGunL_->SetMuzzlePos(muzzlePos);

}

bool BossMech::MoveTo(Vector3 goalPos)
{
    Vector3 vec = goalPos - coreCollider_.lock()->GetTransform()->GetPosition();
    Vector3 vecNormal;
    vec.Normalize(vecNormal);

    //キャラを動かす
    velocity_ = vecNormal * Max_Horizontal_Speed;

    if (vec.LengthSquared() < nearRange_ * nearRange_)
    {
        return true;
    }

    return false;
}

void BossMech::BackTo(AbyssEngine::Vector3 TargetPos)
{
    Vector3 vec = TargetPos - coreCollider_.lock()->GetTransform()->GetPosition();
    Vector3 vecNormal;
    vec.Normalize(vecNormal);

    //キャラを動かす
    velocity_ = -vecNormal * Max_Horizontal_Speed;
}

bool BossMech::CheckTargetDistance(Vector3 targetPos,float range)
{
    Vector3 vec = targetPos - coreCollider_.lock()->GetTransform()->GetPosition();
    return (vec.LengthSquared() < range * range);
}

bool BossMech::TurnToVitesse()
{
    if (const auto& v = targetVitesse_.lock())
    {
        auto vPos = v->GetTransform()->GetPosition();


        //角度判定
        //Y軸を考慮すると大きくズレてしまうのでｙには０を代入
        Vector3 vec = vPos - transform_->GetPosition();
        vec.y = 0;
        vec.Normalize();

        //回転処理
        TurnY(vec);

        Vector3 forward = transform_->GetForward();
        forward.y = 0;

        float dot = vec.Dot(transform_->GetForward());

        return (dot > 0.99f);
    }
}

void BossMech::DrawImGui()
{
    HumanoidWeapon::DrawImGui();

    if (ImGui::TreeNode("BossMech"))
    {
        ImGui::DragFloat("Near Range", &nearRange_, 0.1f);
    }
}
