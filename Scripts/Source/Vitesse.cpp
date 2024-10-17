#include "Vitesse.h"
#include "Actor.h"
#include "Animator.h"
#include "SceneManager.h"
#include "Engine.h"
#include "Input.h"
#include "VitesseState.h"
#include "RenderManager.h"
#include "PlayerSoldier.h"
#include "VitesseAnimState.h"
#include "StaticMesh.h"
#include "CollisionManager.h"
#include "GameCollider.h"
#include "AttackerSystem.h"

#include "ThrusterEffect.h"

using namespace AbyssEngine;

Vitesse::Vitesse() : HumanoidWeapon()
{
}

void Vitesse::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    HumanoidWeapon::Initialize(actor);

    //���f���ǂݍ���
    //model_ = actor_->AddComponent<SkeletalMesh>("./Assets/Models/Vitesse/Vitesse_UE_01_Stand.glb");
    model_ = actor_->AddComponent<SkeletalMesh>("./Assets/Models/Vitesse/Vitesse_UE_01_Stand.gltf");
    
    //�A�j���[�V����������
    AnimationInitialize();

    //����𑕔�������
    leftWeaponModel_ = actor_->AddComponent<StaticMesh>("./Assets/Models/Vitesse/Weapon/Vitesse_GunBlade.gltf");
    rightWeaponModel_ = actor_->AddComponent<StaticMesh>("./Assets/Models/Vitesse/Weapon/Vitesse_GunBlade.gltf");
    model_->SocketAttach(leftWeaponModel_, "rig_J_hand_R");
    model_->SocketAttach(rightWeaponModel_, "rig_J_hand_L");
    leftWeaponModel_->GetSocketData().location_ = Left_Weapon_Offset.pos;
    leftWeaponModel_->GetSocketData().rotation_ = Left_Weapon_Offset.rot;

    rightWeaponModel_->GetSocketData().location_ = Right_Weapon_Offset.pos;
    rightWeaponModel_->GetSocketData().rotation_ = Right_Weapon_Offset.rot;

    //�v���C���[�J�����ݒ�(�v���C���[�Ɛe�q�֌W��)
    //���͂��̂܂܃A�^�b�`���Ă��邪�A��X�Ǝ��̃J��������������
    const auto& c = Engine::sceneManager_->GetActiveScene().InstanceActor("Vitesse Camera");
    camera_ = c->AddComponent<Camera>();
    //c->SetParent(actor_);
    camera_->SetFov(DirectX::XMConvertToRadians(80.0f));
    camera_->SetBaseTargetOffset(Vector3(0.8f, 1.4f, 0));
    camera_->SetTargetOffset(Vector3(0.8f, 1.4f, 0));
    camera_->SetViewTarget(transform_.get());
    camera_->SetEnableDebugController(false);

    //�g�����X�t�H�[���ݒ�
    /*transform_->SetScaleFactor(7.0f);
    transform_->SetPosition(Vector3(0, 3.5f, 4.0f));*/

    //�J�����ݒ�
    camera_->SetArmLength(18.0f);
    camera_->SetTargetOffset({ 12.2f,14.5f,0 });
    camera_->SetBaseArmLength(18.0f);
    camera_->SetBaseTargetOffset({ 12.2f,14.5f,0 });
    defaultCameraLagSpeed_ = camera_->GetCameraLagSpeed();

    //�X�e�[�g�}�V���ݒ�
    stateMachine_ = actor->AddComponent<StateMachine<State<Vitesse>>>();
    stateMachine_->RegisterState(new VitesseState::GroundMove(this));
    stateMachine_->RegisterState(new VitesseState::Flight(this));
    stateMachine_->RegisterState(new VitesseState::TakeOff(this));
    stateMachine_->RegisterState(new VitesseState::Landing(this));
    stateMachine_->RegisterState(new VitesseState::Boarding(this));
    stateMachine_->RegisterState(new VitesseState::HighSpeedFlight(this));
    stateMachine_->RegisterState(new VitesseState::MeleeAttackDash(this));
    stateMachine_->RegisterState(new VitesseState::MeleeAttack(this));

    //�����X�e�[�g�ݒ�
    animStateMachine_->SetState(static_cast<int>(AnimationState::Ground_Move));
    stateMachine_->SetState(static_cast<int>(ActionState::Boarding));

    stateMachine_->SetActive(false);

    //�G�t�F�N�g�ǉ�
    const int unitSize = static_cast<int>(VitesseConstants::Thruster::Location::Installed_Units);
    const auto& units = VitesseConstants::Thrusters;
    for (int i = 0; i < unitSize; ++i)
    {
        thrusters_[i] = actor_->AddComponent<ThrusterEffect>();
        thrusters_[i]->AttachSocket(units[i].socketName_);
        /*thrusters_[i]->SetOffsetPosition(units[i].offsetPos_);
        thrusters_[i]->SetOffsetRotation(units[i].offsetRot_);
        thrusters_[i]->SetOffsetScale(units[i].offsetScale_);*/
    }

    /*thruster_ = actor_->AddComponent<ThrusterEffect>();
    thruster_->AttachSocket("rig_robo_J_backpack_end_R");
    thruster_->SetOffsetPosition(Vector3(-0.4f,-0.1f,0.0f));
    thruster_->SetOffsetRotation(Vector3(0.0f,0.0f,100.0f));
    thruster_->SetOffsetScale(0.3f);*/

    //Engine::renderManager_->ChangeMainCamera(camera_.get());

    //�R���C�_�[�ݒ�
    ColliderInitialize();

    //�A�^�b�J�[�R���|�[�l���g�ݒ�
    AttackerInitialize();
}

void Vitesse::Update()
{
    //�p�C���b�g�����悵�Ă��邩
    PilotUpdate();

    HumanoidWeapon::Update();

    //�^�[�Q�b�g�̍X�V
    TargetAcquisition();

    //�㏸
    RiseInputUpdate();

    animStateMachine_->Update();

    CameraRollUpdate();

    transform_->CalcWorldMatrix();

    ThrusterUpdate();
}

void Vitesse::DrawImGui()
{
    HumanoidWeapon::DrawImGui();

    if (ImGui::TreeNode("Thruster Manager"))
    {
        ImGui::Checkbox("Active Thruster", &activeThruster_);

        if (ImGui::Button("All Fire"))
        {
            for (const auto& t : thrusters_)
            {
                t->Fire();
            }
        }

        if (ImGui::Button("All Stop"))
        {
            for (const auto& t : thrusters_)
            {
                t->Stop();
            }
        }
        ImGui::TreePop();
    }

    stateMachine_->DrawImGui();
}

void Vitesse::AnimationInitialize()
{
    model_->GetAnimator()->AppendAnimations({
                "./Assets/Models/Vitesse/Vitesse_UE_01_Run_F.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Run_R.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Run_L.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Run_B.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_Idle.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_F.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_R.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_L.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_B.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_Up.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_Down.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Landing.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Board_Standby.gltf",//�����p�������荞�ݎp����
                "./Assets/Models/Vitesse/Vitesse_UE_01_Board_Complete.gltf",//��荞�ݎp�����痧���p����
                "./Assets/Models/Vitesse/Vitesse_UE_01_HighSpeedFlight_F.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_HighSpeedFlight_R.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_HighSpeedFlight_L.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_HighSpeedFlight_B.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Dodge_FR.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Dodge_FL.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Ground_Fall.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Slash_Dash_Start.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Slash_Dash_Loop.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Slash_Dash_Loop_R.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Slash_Dash_Loop_L.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Slash_N_1.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Slash_N_2.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Slash_N_2_End.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Slash_N_3.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Slash_N_3_End.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Slash_R_1.gltf",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Flinch.gltf"
        },
        {
            "Run_F",
            "Run_R",
            "Run_L",
            "Run_B",
            "Fly_Idle",
            "Fly_F",
            "Fly_R",
            "Fly_L",
            "Fly_B",
            "Fly_Up",
            "Fly_Down",
            "Fly_Landing",
            "Board_Standby",
            "Board_Complete",
            "HighSpeedFlight_F",
            "HighSpeedFlight_R",
            "HighSpeedFlight_L",
            "HighSpeedFlight_B",
            "Dodge_FR",
            "Dodge_FL",
            "Ground_Fall",
            "Slash_Dash_Start",
            "Slash_Dash_Loop",
            "Slash_Dash_Loop_R",
            "Slash_Dash_Loop_L",
            "Slash_N_1",
            "Slash_N_2",
            "Slash_N_2_End",
            "Slash_N_3",
            "Slash_N_3_End",
            "Slash_R_1",
            "Flinch"
        });

        //���[�v�Đ����Ȃ��悤��
        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Landing))->SetLoopFlag(false);
        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Board_Standby))->SetLoopFlag(false);
        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Board_Complete))->SetLoopFlag(false);

        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Dodge_FR))->SetLoopFlag(false);
        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Dodge_FL))->SetLoopFlag(false);

        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Slash_Dash_Start))->SetLoopFlag(false);
        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Slash_N_1))->SetLoopFlag(false);

        //�Đ����x�̒���
        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Dodge_FR))->SetAnimSpeed(0.5f);
        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Dodge_FL))->SetLoopFlag(0.5f);


        //�n��ړ�
        AnimBlendSpace2D rMoveAnim = AnimBlendSpace2D(model_.get(), "RunMove", static_cast<int>(AnimationIndex::Stand), Vector2(0, 0));
        //�O�A�E�A���A��̏��ɒǉ�
        rMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Run_F), Vector2(0, 1));
        rMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Run_R), Vector2(1, 0));
        rMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Run_L), Vector2(-1, 0));
        rMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Run_B), Vector2(0, -1));
        groundMoveAnimation_ = model_->GetAnimator()->AppendAnimation(rMoveAnim);

        //�󒆈ړ�
#if 0
        AnimBlendSpace2D fMoveAnim = AnimBlendSpace2D(model_.get(), "FlyMove", static_cast<int>(AnimationIndex::Fly_Idle), Vector2(0, 0));
        fMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Fly_F), Vector2(0, 1));
        fMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Fly_R), Vector2(1, 0));
        fMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Fly_L), Vector2(-1, 0));
        fMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Fly_B), Vector2(0, -1));
#else
        //��s�̃u�����h���[�V�����o�^
        AnimBlendSpace1D fMoveAnim1D = AnimBlendSpace1D(model_.get(), "FlyMoveUpDown", static_cast<int>(AnimationIndex::Flight_Idle), static_cast<int>(AnimationIndex::Flight_Up));
        fMoveAnim1D.AddBlendAnimation(static_cast<int>(AnimationIndex::Flight_Down), -1.0f);
        auto* f1d = model_->GetAnimator()->AppendAnimation(fMoveAnim1D);
        f1d->SetMinWeight(-1.0f);

        AnimBlendSpace2D fMoveAnim2D = AnimBlendSpace2D(model_.get(), "FlyMove2D", static_cast<int>(AnimationIndex::Flight_Idle), Vector2(0, 0));
        fMoveAnim2D.AddBlendAnimation(static_cast<int>(AnimationIndex::Flight_F), Vector2(0, 1));
        fMoveAnim2D.AddBlendAnimation(static_cast<int>(AnimationIndex::Flight_R), Vector2(1, 0));
        fMoveAnim2D.AddBlendAnimation(static_cast<int>(AnimationIndex::Flight_L), Vector2(-1, 0));
        fMoveAnim2D.AddBlendAnimation(static_cast<int>(AnimationIndex::Flight_B), Vector2(0, -1));
        auto* f2d = model_->GetAnimator()->AppendAnimation(fMoveAnim2D);

        AnimBlendSpaceFlyMove fMoveAnim = AnimBlendSpaceFlyMove(model_.get(), "FlyMove3D", f2d, f1d);
        flightAnimation_ = model_->GetAnimator()->AppendAnimation(fMoveAnim);


        //������s�̃u�����h���[�V�����o�^
        AnimBlendSpace2D highSpeedFMoveAnim2D = AnimBlendSpace2D(model_.get(), "HighSpeedFlyMove2D", static_cast<int>(AnimationIndex::Flight_Idle), Vector2(0, 0));
        highSpeedFMoveAnim2D.AddBlendAnimation(static_cast<int>(AnimationIndex::HighSpeedFlight_F), Vector2(0, 1));
        highSpeedFMoveAnim2D.AddBlendAnimation(static_cast<int>(AnimationIndex::HighSpeedFlight_R), Vector2(1, 0));
        highSpeedFMoveAnim2D.AddBlendAnimation(static_cast<int>(AnimationIndex::HighSpeedFlight_L), Vector2(-1, 0));
        highSpeedFMoveAnim2D.AddBlendAnimation(static_cast<int>(AnimationIndex::HighSpeedFlight_B), Vector2(0, -1));
        auto* hf2d = model_->GetAnimator()->AppendAnimation(highSpeedFMoveAnim2D);

        AnimBlendSpaceFlyMove hfMoveAnim = AnimBlendSpaceFlyMove(model_.get(), "HighSpeedFlyMove3D", hf2d, f1d);
        highSpeedFlightAnimation_ = model_->GetAnimator()->AppendAnimation(hfMoveAnim);
#endif // 0

        model_->GetAnimator()->PlayAnimation(static_cast<int>(AnimationIndex::Run_Move));

        //�A�j���[�V�����X�e�[�g�}�V�[���ݒ�
        animStateMachine_ = actor_->AddComponent<StateMachine<State<Animator>>>();
        animStateMachine_->RegisterState(new VitesseAnimState::AnimDefault(model_->GetAnimator().get()));
        animStateMachine_->RegisterState(new VitesseAnimState::AnimGroundMove(model_->GetAnimator().get()));
        animStateMachine_->RegisterState(new VitesseAnimState::AnimFlight(model_->GetAnimator().get()));
        animStateMachine_->RegisterState(new VitesseAnimState::AnimHighSpeedFlight(model_->GetAnimator().get()));
}

void Vitesse::ThrusterInfluenceVelocity()
{
    if (!activeThruster_)return;
    
    Vector3 vNormal;
    velocity_.Normalize(vNormal);

    float speed = velocity_.x * velocity_.x + velocity_.z * velocity_.z;
    if (flightMode_)
    {
        /*if (speed)
        {
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_R_U)]->Fire();
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_R_D)]->Fire();
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_L_U)]->Fire();
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_L_D)]->Fire();
        }*/
        const float power = 0.4f;
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackLeg_R_U)]->Fire(power);
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackLeg_R_M)]->Fire(power);
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackLeg_R_D)]->Fire(power);
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackLeg_L_U)]->Fire(power);
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackLeg_L_M)]->Fire(power);
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackLeg_L_D)]->Fire(power);
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Waist_R)]->Fire(power);
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Waist_L)]->Fire(power);
    }

    if (velocity_.y > 0)
    {
        const float power = 1.0f;
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackLeg_R_U)]->Fire(power);
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackLeg_R_M)]->Fire(power);
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackLeg_R_D)]->Fire(power);
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackLeg_L_U)]->Fire(power);
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackLeg_L_M)]->Fire(power);
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackLeg_L_D)]->Fire(power);
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Waist_R)]->Fire(power);
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Waist_L)]->Fire(power);
    }

    if (speed > 0.01f)
    {
        if (!slowDown_)
        {
            float power = 0.6f + moveDirection_.z * 0.4f;
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_R_U)]->Fire(power);
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_R_D)]->Fire(power);
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_L_U)]->Fire(power);
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_L_D)]->Fire(power);
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Knee_R_R)]->Stop();
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Knee_R_L)]->Stop();
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Knee_L_R)]->Stop();
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Knee_L_L)]->Stop();
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Chest_R)]->Stop();
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Chest_L)]->Stop();
        }
        else
        {
            float power = 0.6f + fabsf(moveDirection_.z) * 0.4f;
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_R_U)]->Stop();
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_R_D)]->Stop();
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_L_U)]->Stop();
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_L_D)]->Stop();
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Knee_R_R)]->Fire(power);
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Knee_R_L)]->Fire(power);
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Knee_L_R)]->Fire(power);
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Knee_L_L)]->Fire(power);
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Chest_R)]->Fire(power);
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Chest_L)]->Fire(power);
        }

        if (moveDirection_.x > 0.05f)
        {
            float power = 0.6f + moveDirection_.z * 0.4f;
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Shoulder_R_1)]->Stop();
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Shoulder_L_1)]->Fire(power);
        }
        else if (moveDirection_.x < -0.05f)
        {
            float power = 0.6f + fabsf(moveDirection_.z) * 0.4f;
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Shoulder_R_1)]->Fire(power);
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Shoulder_L_1)]->Stop();
        }
        else
        {
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Shoulder_R_1)]->Stop();
            thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Shoulder_L_1)]->Stop();
        }
    }
    else
    {
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_R_U)]->Stop();
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_R_D)]->Stop();
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_L_U)]->Stop();
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::BackPack_L_D)]->Stop();
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Knee_R_R)]->Stop();
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Knee_R_L)]->Stop();
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Knee_L_R)]->Stop();
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Knee_L_L)]->Stop();
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Chest_R)]->Stop();
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Chest_L)]->Stop();
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Shoulder_R_1)]->Stop();
        thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Shoulder_L_1)]->Stop();
    }
}

void Vitesse::ThrusterAllStop()
{
    for (const auto& t : thrusters_)
    {
        t->Stop();
    }
}

void Vitesse::ColliderInitialize()
{
    //��炢����R���C�_�[�ݒ�
    {
        std::vector<std::shared_ptr<HitCollider>> hitColliders =
        {
            //�����͂Ȃ��Ă悳��
            //AddHitCollider(Vector3::Zero, 3.0f, "HitCollider_UpChest", model_, "rig_J_upbody2");
            AddHitCollider(Vector3::Zero, 2.0f, "HitCollider_LowChest", model_, "rig_J_upbody01"),

            AddHitCollider(Vector3::Zero, 2.0f, "HitCollider_Head", model_, "rig_J_head"),

            AddHitCollider(Vector3(-0.1f, 0, 0), 1.5f, "HitCollider_Uparm_R", model_, "rig_J_uparm_R"),
            AddHitCollider(Vector3::Zero, 1.0f, "HitCollider_Lowarm_R", model_, "rig_J_lowarm_R"),
            AddHitCollider(Vector3::Zero, 1.0f, "HitCollider_Hand_R", model_, "rig_J_hand_R"),

            AddHitCollider(Vector3(0.1f, 0, 0), 1.5f, "HitCollider_Uparm_L", model_, "rig_J_uparm_L"),
            AddHitCollider(Vector3::Zero, 1.0f, "HitCollider_Lowarm_L", model_, "rig_J_lowarm_L"),
            AddHitCollider(Vector3::Zero, 1.0f, "HitCollider_Hand_L", model_, "rig_J_hand_L"),

            AddHitCollider(Vector3::Zero, 1.0f, "HitCollider_Hip", model_, "rig_J_root"),

            AddHitCollider(Vector3::Zero, 1.0f, "HitCollider_Upleg_R", model_, "rig_J_upleg_R"),
            AddHitCollider(Vector3::Zero, 1.0f, "HitCollider_Lowleg_R", model_, "rig_J_lowleg_R"),
            AddHitCollider(Vector3::Zero, 1.0f, "HitCollider_Foot_R", model_, "rig_J_foot_R"),

            AddHitCollider(Vector3::Zero, 1.0f, "HitCollider_Upleg_L", model_, "rig_J_upleg_L"),
            AddHitCollider(Vector3::Zero, 1.0f, "HitCollider_Lowleg_L", model_, "rig_J_lowleg_L"),
            AddHitCollider(Vector3::Zero, 1.0f, "HitCollider_Foot_L", model_, "rig_J_foot_L"),
        };
        for (const auto& collider : hitColliders)
        {
            collider->ReplaceTag(Collider::Tag::Player);
        }
    }

    //�U������R���C�_�[�ݒ�
    {
        //���̕����Ɉ��Ԋu�ŃR���C�_�[��ݒu���Ă���
        Vector3 shaftVecL = { 1.0f,0.4f,0.1f };
        shaftVecL.Normalize();
        Vector3 shaftVecR = shaftVecL * -1.0f;

        //������
        std::vector<std::shared_ptr<AttackCollider>> atkCollidersL =
        {
            AddAttackCollider(Vector3::Zero, 4.0f, "AtkCol_Weapon_R_1", model_, "rig_J_hand_R"),
            AddAttackCollider(Vector3::Zero, 4.0f, "AtkCol_Weapon_R_2", model_, "rig_J_hand_R"),
            AddAttackCollider(Vector3::Zero, 4.0f, "AtkCol_Weapon_R_3", model_, "rig_J_hand_R"),
            AddAttackCollider(Vector3::Zero, 4.0f, "AtkCol_Weapon_R_4", model_, "rig_J_hand_R"),
        };


        int i = 0;
        for (const auto& collider : atkCollidersL)
        {
            collider->ReplaceTag(Collider::Tag::Player);
            collider->GetTransform()->SetLocalPosition(shaftVecR * i);

            //�ݒ肵���R���C�_�[��o�^
            lWeaponAtkColliderList_.emplace_back(collider);

            ++i;
        }

        //�E����
        std::vector<std::shared_ptr<AttackCollider>> atkCollidersR =
        {
            AddAttackCollider(Vector3::Zero, 4.0f, "AtkCol_Weapon_L_1", model_, "rig_J_hand_L"),
            AddAttackCollider(Vector3::Zero, 4.0f, "AtkCol_Weapon_L_2", model_, "rig_J_hand_L"),
            AddAttackCollider(Vector3::Zero, 4.0f, "AtkCol_Weapon_L_3", model_, "rig_J_hand_L"),
            AddAttackCollider(Vector3::Zero, 4.0f, "AtkCol_Weapon_L_4", model_, "rig_J_hand_L"),
        };
        //���̕����Ɉ��Ԋu�ŃR���C�_�[��ݒu���Ă���
        i = 0;
        for (const auto& collider : atkCollidersR)
        {
            collider->ReplaceTag(Collider::Tag::Player);
            collider->GetTransform()->SetLocalPosition(shaftVecL * i);

            //�ݒ肵���R���C�_�[��o�^
            rWeaponAtkColliderList_.emplace_back(collider);
            ++i;
        }
    }

    //�����o������ݒ�
    {
        std::vector<std::shared_ptr<TerrainCollider>> terrainColliders =
        {
            AddTerrainCollider(Vector3(-0.4f,0,0), 7.68f, "TerrainCollider_Center", model_, "rig_J_upbody01")

        };

        for (const auto& collider : terrainColliders)
        {
            collider->ReplaceTag(Collider::Tag::Player);
        }
    }
}

void Vitesse::AttackerInitialize()
{
    attackerSystem_ = actor_->AddComponent<AttackerSystem>();
    //�e��R���C�_�[�o�^
    for (const auto& collider : lWeaponAtkColliderList_)
    {
        attackerSystem_->RegistCollider(collider);
    }
    for (const auto& collider : rWeaponAtkColliderList_)
    {
        attackerSystem_->RegistCollider(collider);
    }

    //�a��N�U��
    {
        AttackData atkData;
        atkData.power_ = 10.0f;//�U����
        for (const auto& collider : rWeaponAtkColliderList_)
        {
            atkData.attackColliderList_.emplace_back(collider);
        }
        atkData.duration_ = 1.0f;//��������
        //atkData.staggerValue_
        atkData.maxHits_ = 1;//�U���ő�q�b�g��
        atkData.hitStop_ = 0.1f;//�q�b�g�X�g�b�v����

        //�A�^�b�J�[�V�X�e���ɓo�^
        attackerSystem_->RegistAttackData("Slash_N_1", atkData);
    }
    
}

void Vitesse::Dodge(Vector3 direction)
{
    //�������̐ݒ�
    direction.Normalize();
    dodgeDirection_ = direction;

    //���݂̑��x���������։���
    velocity_ = direction * dodgeSpeed_;
}

void Vitesse::StepMove(AbyssEngine::Vector3 moveDirection, float speed)
{
    //�������̐ݒ�
    moveDirection.Normalize();
    dodgeDirection_ = moveDirection;

    //���݂̑��x���������։���
    velocity_ = moveDirection * speed;
}

void Vitesse::OnCollision(const std::shared_ptr<AbyssEngine::Collider>& hitCollider, Collision::IntersectionResult result)
{
    
}

Vector3 Vitesse::ToTarget()
{
    Vector3 result = {};

    //���@�̒��S����v�Z����
    //���S�̃W���C���g����v�Z����
    Matrix mat = model_->FindSocket("rig_J_upbody2") * transform_->GetWorldMatrix();
    Vector3 pos = { mat._41,mat._42,mat._43, };

    // �^�[�Q�b�g�̈ʒu���Z�o
    if (const auto& target = lockonTarget_.lock())
    {
        //�^�[�Q�b�g�ւ̕�����
        Vector3 toTarget = target->GetTransform()->GetPosition() - pos;
        result = toTarget;
    }
    else
    {
        //�^�[�Q�b�g�����Ȃ��Ȃ�A�J���������Ă��������
        result = camera_->GetTransform()->GetForward();
    }

    return result;
}

void Vitesse::ChangeActionState(const ActionState& state)
{
    stateMachine_->ChangeState(static_cast<int>(state));
}

void Vitesse::ChangeAnimationState(const AnimationState& state)
{
    animStateMachine_->ChangeState(static_cast<int>(state));
}

void Vitesse::GetOnBoardPilot(const std::shared_ptr<Soldier>& pilot)
{
    //�J������ύX
    Camera::ChangeMainCamera(camera_.get());

    //�p�C���b�g�ݒ�
    pilot_ = pilot;
}

void Vitesse::UpdateInputMove()
{
    if (!camera_->GetIsMainCamera())return;

    //��s����Y���ɂ������悤�ɂ���
    if (flightMode_)
    {
        moveVec_ = camera_->ConvertTo3DVectorFromCamera(Input::GameSupport::GetMoveVector());
        moveVec_.y += Input::GameSupport::GetRiseButton() ? 1.0f : 0.0f;
    }
    else
    {
        moveVec_ = camera_->ConvertTo2DVectorFromCamera(Input::GameSupport::GetMoveVector());
    }
}

void Vitesse::ToGroundMode()
{
    HumanoidWeapon::ToGroundMode();

    ThrusterAllStop();
}

//�^�[�Q�b�g�ƂȂ�R���C�_�[��⑫����
void Vitesse::TargetAcquisition()
{
    //�S�q�b�g�R���C�_�[�Ɠ��ϒl�ƃX�N���[�����W����⑫����^�[�Q�b�g���擾����
    //�⑫�ł���R���C�_�[����������ꍇ�͂��߂�����⑫
    const auto& colliderList = Engine::collisionManager_->GetHitColliderList();

    //���݃^�[�Q�b�g�ɐݒ肳��Ă���A�N�^�[�܂ł̃X�N���[������
    float nearestDistSq = FLT_MAX;

    //�^�[�Q�b�g�����m�������̃t���O
    bool findTarget = false;

    //�^�[�Q�b�g���ύX�t���O�����Z�b�g���Ă���
    changeLockonTarget_ = false;

    for (auto& collider : colliderList)
    {
        if (const auto& col = collider.lock())
        {
            if (!col->GetEnabled())continue;
            if (!col->GetLockonTarget())continue;
            if (col->GetTag() != static_cast<unsigned int>(Collider::Tag::Enemy))continue;

            const Vector3 colPos = col->GetTransform()->GetPosition();
            const Vector3 pos = transform_->GetPosition();

            const Vector3 toCollider = Vector3::Normalize(colPos - pos);
            const Vector3 cameraForward = Vector3::Normalize(camera_->GetFocus() - camera_->GetEye());
            
            //���ϒl�����̃R���C�_�[�͊m���ɃJ��������ʂ�Ȃ��̂ŏȂ�
            float dot = cameraForward.Dot(toCollider);
            if (dot < 0)continue;

            //�X�N���[�����W�����ʒ�������ǂ̒��x�̈ʒu�ɂ��邩����
            Vector2 colliderScreenPos = camera_->WorldToScreenPosition(colPos);
            D3D11_VIEWPORT viewport;
            DXSystem::GetViewport(1, &viewport);
            Vector2 center = { viewport.Width / 2, viewport.Height / 2 };

            const Vector2 centerToCollider = colliderScreenPos - center;
            const float distSq = centerToCollider.LengthSquared();

            //��������
            if (distSq < lockRadius_ * lockRadius_)
            {
                findTarget = true;

                //�^�[�Q�b�g�����łɐݒ肳��Ă��邩
                if (lockonTarget_.lock())
                {
                    //���݂̃^�[�Q�b�g�Ƃ̋������r�A�K�v�Ȃ�^�[�Q�b�g�X�V
                    if (nearestDistSq > distSq)
                    {
                        //�^�[�Q�b�g��ݒ�
                        lockonTarget_ = col->GetActor();
                        nearestDistSq = distSq;

                        //�^�[�Q�b�g�ύX�t���O�𗧂Ă�
                        //changeLockonTarget_ = true;
                    }
                }
                else
                {
                    //�^�[�Q�b�g��ݒ�
                    lockonTarget_ = col->GetActor();
                    nearestDistSq = distSq;

                    //�^�[�Q�b�g�ύX�t���O�𗧂Ă�
                    changeLockonTarget_ = true;
                }
            }
        }
    }

    //�^�[�Q�b�g��������Ȃ������ꍇ�͊����̃^�[�Q�b�g���X�V����
    if (!findTarget)
    {
        if (const auto& actor = lockonTarget_.lock())
        {
            const Vector3 targetPos = actor->GetTransform()->GetPosition();
            const Vector3 pos = transform_->GetPosition();

            const Vector3 toCollider = Vector3::Normalize(targetPos - pos);
            const Vector3 cameraForward = Vector3::Normalize(camera_->GetFocus() - camera_->GetEye());

            //���ϒl�����Ȃ烊�Z�b�g����
            float dot = cameraForward.Dot(toCollider);
            if (dot < 0)lockonTarget_.reset();
            else
            {
                //�X�N���[�����W�����ʒ�������ǂ̒��x�̈ʒu�ɂ��邩����
                Vector2 targetScreenPos = camera_->WorldToScreenPosition(targetPos);
                D3D11_VIEWPORT viewport;
                DXSystem::GetViewport(1, &viewport);
                Vector2 targetViewport = { targetScreenPos.x/viewport.Width, targetScreenPos.y/viewport.Height};
                
                //�r���[�|�[�g���W�����ʊO�ɏo�Ă��Ȃ������ׂ�
                if ((targetViewport.x > 0 && targetViewport.x < 1) &&
                    (targetViewport.y > 0 && targetViewport.y < 1))
                {
                    //��ʓ��ɂ���̂ň��������⑫
                }
                else
                {
                    //��ʊO�ɏo�Ă���̂Ń��Z�b�g
                    lockonTarget_.reset();
                }
            }
        }
    }
}

void Vitesse::PlayAnimation(AnimationIndex index, float transTime,float startTime)
{
    model_->GetAnimator()->PlayAnimation(static_cast<int>(index), transTime, startTime);
}

void Vitesse::RadialBlurFromTarget()
{
    if (const auto& target = lockonTarget_.lock())
    {
        const Vector3 targetViewportPos = camera_->WorldToViewportPosition(target->GetTransform()->GetPosition());

        auto& effectData = Engine::renderManager_->GetBufferEffects().data_;
        effectData.radialBlurUvOffset_[0] = targetViewportPos.x;
        effectData.radialBlurUvOffset_[1] = targetViewportPos.y;
    }
    else
    {
        auto& effectData = Engine::renderManager_->GetBufferEffects().data_;
        effectData.radialBlurUvOffset_[0] = 0.5f;
        effectData.radialBlurUvOffset_[1] = 0.5f;
    }
}

bool Vitesse::ApplyDamage(const AttackParameter& param, DamageResult* damageResult)
{
    bool hit = Character::ApplyDamage(param,damageResult);

    //�m�b�N�o�b�N����
    if (hit)
    {
        if (param.knockback_ > 0.001f)
        {
            
        }
    }

    return hit;
}

void Vitesse::CameraRollUpdate()
{
    //���͒l�擾
    Vector2 input = Input::GameSupport::GetCameraRollVector();

    auto r = camera_->GetTransform()->GetRotation();
    const float rollSpeed = cameraRollSpeed_ * actor_->GetDeltaTime();
    r.x = r.x + input.y * rollSpeed;
    r.y = r.y + input.x * rollSpeed;
    camera_->GetTransform()->SetRotation(r);
}

void Vitesse::ThrusterUpdate()
{
    //�G�t�F�N�g�X�V
    for (const auto& t : thrusters_)
    {
        t->UpdateInjection();
        t->UpdateTransform();
    }
}

void Vitesse::PilotUpdate()
{
    const auto& pilot = pilot_.lock();
    if (pilot && pilot->GetVitesseOnBoard())
    {
        stateMachine_->SetActive(true);
    }
    else
    {
        stateMachine_->SetActive(false);
    }
}

void Vitesse::RiseInputUpdate()
{
    if (flightMode_ && Input::GameSupport::GetRiseButton())
    {
        Rise(riseSpeed_ * actor_->GetDeltaTime());
    }
}
