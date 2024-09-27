#include "BossMech.h"
#include "Actor.h"
#include "SkeletalMesh.h"
#include "GameCollider.h"

using namespace AbyssEngine;

void BossMech::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    HumanoidWeapon::Initialize(actor);

    enableGravity_ = false;

    model_ = actor->AddComponent<AbyssEngine::SkeletalMesh>("./Assets/Models/Enemy/Boss/Mech_Idle.glb");
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

    transform_->SetScaleFactor(35.0f);

   /* const auto& coll = AddHitCollider(Vector3::Zero, 10.0f, "Collider_Chest");
    coll->AttachModel(model_, "spine_02");*/

    //コライダー設定
    ColliderInitialize();
}

void BossMech::ColliderInitialize()
{
    const std::shared_ptr<SphereCollider> collider[] =
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
        AddHitCollider(Vector3::Zero,  8.0f, "Collider_Knee_R",          model_, "calf_r"),
        AddHitCollider(Vector3::Zero,  6.0f, "Collider_Foot_R",          model_, "foot_r"),

        AddHitCollider(Vector3::Zero,  6.0f, "Collider_Thigh_L",         model_, "thigh_l"),
        AddHitCollider(Vector3::Zero,  8.0f, "Collider_Knee_L",          model_, "calf_l"),
        AddHitCollider(Vector3::Zero,  6.0f, "Collider_Foot_L",          model_, "foot_l")
    };
}
