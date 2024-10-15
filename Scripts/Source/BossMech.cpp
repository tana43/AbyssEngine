#include "BossMech.h"
#include "Actor.h"
#include "SkeletalMesh.h"
#include "GameCollider.h"
#include "BehaviorTree.h"
#include "ActionDerived.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Vitesse.h"

using namespace AbyssEngine;

#define Ai_SelectRule BehaviorTree<BossMech>::SelectRule

void BossMech::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    HumanoidWeapon::Initialize(actor);

    //�p�����[�^�̐ݒ�
    health_ = 1000.0f;
    Max_Health = 1000.0f;

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

    //�R���C�_�[�ݒ�
    ColliderInitialize();

    //AI������
    BehaviorTreeInitialize();

    //�^�[�Q�b�g�ɂȂ郔�B�e�X���擾
    const auto& vitesseActor = Engine::sceneManager_->GetActiveScene().Find("Vitesse");
    if (const auto& a = vitesseActor.lock())
    {
        targetVitesse_ = a->GetComponent<Vitesse>();
    }
}

void BossMech::ColliderInitialize()
{
    //��炢����ݒ�
    {
        const std::shared_ptr<HitCollider> collider[] =
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

            //���
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


        //���b�N�I���\�ȃR���C�_�[��ݒ�
        std::string lockonCollider[] = {
            "Collider_Chest",
            "Collider_Head",
            "Collider_Lowerarm_R",
            "Collider_Lowerarm_L",
            "Collider_Hip",
            "Collider_Knee_R",
            "Collider_Knee_L"
        };

        //�^�O��ݒ�
        for (const auto& col : collider)
        {
            col->ReplaceTag(Collider::Tag::Enemy);

            //���b�N�I���\�ȃR���C�_�[��ݒ�
            for (const auto& lockon : lockonCollider)
            {
                if (col->GetActor()->name_ == lockon)
                {
                    col->SetLockonTarget(true);
                }
            }
        }
    }

    //�����o������ݒ�
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

            //���
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


        //���b�N�I���\�ȃR���C�_�[��ݒ�
        std::string lockonCollider[] = {
            "Collider_Chest",
            "Collider_Head",
            "Collider_Lowerarm_R",
            "Collider_Lowerarm_L",
            "Collider_Hip",
            "Collider_Knee_R",
            "Collider_Knee_L"
        };

        //�^�O��ݒ�
        for (const auto& col : collider)
        {
            col->ReplaceTag(Collider::Tag::Enemy);
        }
    }

}

void BossMech::BehaviorTreeInitialize()
{
    //�r�w�C�r�A�c���[
    aiTree_ = actor_->AddComponent<BehaviorTree<BossMech>>();
    aiTree_->SetOwner(std::static_pointer_cast<BossMech>(shared_from_this()));

    //BehaviorTree���\�z
    aiTree_->AddNode("", "Root", 0, Ai_SelectRule::Priority, nullptr, nullptr);

    //�퓬
    aiTree_->AddNode("Root", "Battle", 0, Ai_SelectRule::Sequence, new MechBattleJudgment(this), nullptr);
    //��@
    aiTree_->AddNode("Root", "Scout", 1, Ai_SelectRule::Sequence, nullptr, nullptr);

    //�퓬�m�[�h
    aiTree_->AddNode("Battle", "Attack", 0, Ai_SelectRule::Non, new MechRunAttackJudgment(this), new MechRunAttackAction(this));
    //aiTree_->AddNode("Battle", "Dodge", 1, Ai_SelectRule::Non, new DodgeJudgment(this), new BotSideDodgeAction(this));

    //��@�m�[�h
    aiTree_->AddNode("Scout", "Idle", 1, Ai_SelectRule::Non, nullptr, new MechIdleAction(this));
}
