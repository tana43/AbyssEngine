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

    //モデル読み込み
    //model_ = actor_->AddComponent<SkeletalMesh>("./Assets/Models/Vitesse/Vitesse_UE_01_Stand.glb");
    model_ = actor_->AddComponent<SkeletalMesh>("./Assets/Models/Vitesse/Vitesse_UE_01_Stand.gltf");
    
    //アニメーション初期化
    AnimationInitialize();

    //武器を装備させる
    leftWeaponModel_ = actor_->AddComponent<StaticMesh>("./Assets/Models/Vitesse/Weapon/Vitesse_GunBlade.gltf");
    rightWeaponModel_ = actor_->AddComponent<StaticMesh>("./Assets/Models/Vitesse/Weapon/Vitesse_GunBlade.gltf");
    model_->SocketAttach(leftWeaponModel_, "rig_J_hand_R");
    model_->SocketAttach(rightWeaponModel_, "rig_J_hand_L");
    leftWeaponModel_->GetSocketData().location_ = Left_Weapon_Offset.pos;
    leftWeaponModel_->GetSocketData().rotation_ = Left_Weapon_Offset.rot;

    rightWeaponModel_->GetSocketData().location_ = Right_Weapon_Offset.pos;
    rightWeaponModel_->GetSocketData().rotation_ = Right_Weapon_Offset.rot;

    //プレイヤーカメラ設定(プレイヤーと親子関係に)
    //今はそのままアタッチしているが、後々独自のカメラ挙動をつくる
    const auto& c = Engine::sceneManager_->GetActiveScene().InstanceActor("Vitesse Camera");
    camera_ = c->AddComponent<Camera>();
    //c->SetParent(actor_);
    camera_->SetFov(DirectX::XMConvertToRadians(80.0f));
    camera_->SetBaseTargetOffset(Vector3(0.8f, 1.4f, 0));
    camera_->SetTargetOffset(Vector3(0.8f, 1.4f, 0));
    camera_->SetViewTarget(transform_.get());
    camera_->SetEnableDebugController(false);

    //トランスフォーム設定
    /*transform_->SetScaleFactor(7.0f);
    transform_->SetPosition(Vector3(0, 3.5f, 4.0f));*/

    //カメラ設定
    camera_->SetArmLength(18.0f);
    camera_->SetTargetOffset({ 12.2f,14.5f,0 });
    camera_->SetBaseArmLength(18.0f);
    camera_->SetBaseTargetOffset({ 12.2f,14.5f,0 });
    defaultCameraLagSpeed_ = camera_->GetCameraLagSpeed();

    //ステートマシン設定
    stateMachine_ = actor->AddComponent<StateMachine<State<Vitesse>>>();
    stateMachine_->RegisterState(new VitesseState::GroundMove(this));
    stateMachine_->RegisterState(new VitesseState::Flight(this));
    stateMachine_->RegisterState(new VitesseState::TakeOff(this));
    stateMachine_->RegisterState(new VitesseState::Landing(this));
    stateMachine_->RegisterState(new VitesseState::Boarding(this));
    stateMachine_->RegisterState(new VitesseState::HighSpeedFlight(this));
    stateMachine_->RegisterState(new VitesseState::MeleeAttackDash(this));
    stateMachine_->RegisterState(new VitesseState::MeleeAttack(this));

    //初期ステート設定
    animStateMachine_->SetState(static_cast<int>(AnimationState::Ground_Move));
    stateMachine_->SetState(static_cast<int>(ActionState::Boarding));

    stateMachine_->SetActive(false);

    //エフェクト追加
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

    //コライダー設定
    ColliderInitialize();

    //アタッカーコンポーネント設定
    AttackerInitialize();
}

void Vitesse::Update()
{
    //パイロットが搭乗しているか
    PilotUpdate();

    HumanoidWeapon::Update();

    //ターゲットの更新
    TargetAcquisition();

    //上昇
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
                "./Assets/Models/Vitesse/Vitesse_UE_01_Board_Standby.gltf",//立ち姿勢から乗り込み姿勢へ
                "./Assets/Models/Vitesse/Vitesse_UE_01_Board_Complete.gltf",//乗り込み姿勢から立ち姿勢へ
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

        //ループ再生しないように
        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Landing))->SetLoopFlag(false);
        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Board_Standby))->SetLoopFlag(false);
        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Board_Complete))->SetLoopFlag(false);

        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Dodge_FR))->SetLoopFlag(false);
        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Dodge_FL))->SetLoopFlag(false);

        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Slash_Dash_Start))->SetLoopFlag(false);
        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Slash_N_1))->SetLoopFlag(false);

        //再生速度の調整
        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Dodge_FR))->SetAnimSpeed(0.5f);
        model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Dodge_FL))->SetLoopFlag(0.5f);


        //地上移動
        AnimBlendSpace2D rMoveAnim = AnimBlendSpace2D(model_.get(), "RunMove", static_cast<int>(AnimationIndex::Stand), Vector2(0, 0));
        //前、右、左、後の順に追加
        rMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Run_F), Vector2(0, 1));
        rMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Run_R), Vector2(1, 0));
        rMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Run_L), Vector2(-1, 0));
        rMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Run_B), Vector2(0, -1));
        groundMoveAnimation_ = model_->GetAnimator()->AppendAnimation(rMoveAnim);

        //空中移動
#if 0
        AnimBlendSpace2D fMoveAnim = AnimBlendSpace2D(model_.get(), "FlyMove", static_cast<int>(AnimationIndex::Fly_Idle), Vector2(0, 0));
        fMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Fly_F), Vector2(0, 1));
        fMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Fly_R), Vector2(1, 0));
        fMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Fly_L), Vector2(-1, 0));
        fMoveAnim.AddBlendAnimation(static_cast<int>(AnimationIndex::Fly_B), Vector2(0, -1));
#else
        //飛行のブレンドモーション登録
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


        //高速飛行のブレンドモーション登録
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

        //アニメーションステートマシーン設定
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
    //喰らい判定コライダー設定
    {
        std::vector<std::shared_ptr<HitCollider>> hitColliders =
        {
            //こいつはなくてよさげ
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

    //攻撃判定コライダー設定
    {
        //この方向に一定間隔でコライダーを設置していく
        Vector3 shaftVecL = { 1.0f,0.4f,0.1f };
        shaftVecL.Normalize();
        Vector3 shaftVecR = shaftVecL * -1.0f;

        //左武器
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

            //設定したコライダーを登録
            lWeaponAtkColliderList_.emplace_back(collider);

            ++i;
        }

        //右武器
        std::vector<std::shared_ptr<AttackCollider>> atkCollidersR =
        {
            AddAttackCollider(Vector3::Zero, 4.0f, "AtkCol_Weapon_L_1", model_, "rig_J_hand_L"),
            AddAttackCollider(Vector3::Zero, 4.0f, "AtkCol_Weapon_L_2", model_, "rig_J_hand_L"),
            AddAttackCollider(Vector3::Zero, 4.0f, "AtkCol_Weapon_L_3", model_, "rig_J_hand_L"),
            AddAttackCollider(Vector3::Zero, 4.0f, "AtkCol_Weapon_L_4", model_, "rig_J_hand_L"),
        };
        //この方向に一定間隔でコライダーを設置していく
        i = 0;
        for (const auto& collider : atkCollidersR)
        {
            collider->ReplaceTag(Collider::Tag::Player);
            collider->GetTransform()->SetLocalPosition(shaftVecL * i);

            //設定したコライダーを登録
            rWeaponAtkColliderList_.emplace_back(collider);
            ++i;
        }
    }

    //押し出し判定設定
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
    //各種コライダー登録
    for (const auto& collider : lWeaponAtkColliderList_)
    {
        attackerSystem_->RegistCollider(collider);
    }
    for (const auto& collider : rWeaponAtkColliderList_)
    {
        attackerSystem_->RegistCollider(collider);
    }

    //斬撃N攻撃
    {
        AttackData atkData;
        atkData.power_ = 10.0f;//攻撃力
        for (const auto& collider : rWeaponAtkColliderList_)
        {
            atkData.attackColliderList_.emplace_back(collider);
        }
        atkData.duration_ = 1.0f;//持続時間
        //atkData.staggerValue_
        atkData.maxHits_ = 1;//攻撃最大ヒット回数
        atkData.hitStop_ = 0.1f;//ヒットストップ時間

        //アタッカーシステムに登録
        attackerSystem_->RegistAttackData("Slash_N_1", atkData);
    }
    
}

void Vitesse::Dodge(Vector3 direction)
{
    //回避方向の設定
    direction.Normalize();
    dodgeDirection_ = direction;

    //現在の速度を回避方向へ加速
    velocity_ = direction * dodgeSpeed_;
}

void Vitesse::StepMove(AbyssEngine::Vector3 moveDirection, float speed)
{
    //回避方向の設定
    moveDirection.Normalize();
    dodgeDirection_ = moveDirection;

    //現在の速度を回避方向へ加速
    velocity_ = moveDirection * speed;
}

void Vitesse::OnCollision(const std::shared_ptr<AbyssEngine::Collider>& hitCollider, Collision::IntersectionResult result)
{
    
}

Vector3 Vitesse::ToTarget()
{
    Vector3 result = {};

    //自機の中心から計算する
    //中心のジョイントから計算する
    Matrix mat = model_->FindSocket("rig_J_upbody2") * transform_->GetWorldMatrix();
    Vector3 pos = { mat._41,mat._42,mat._43, };

    // ターゲットの位置を算出
    if (const auto& target = lockonTarget_.lock())
    {
        //ターゲットへの方向へ
        Vector3 toTarget = target->GetTransform()->GetPosition() - pos;
        result = toTarget;
    }
    else
    {
        //ターゲットがいないなら、カメラが見ている方向へ
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
    //カメラを変更
    Camera::ChangeMainCamera(camera_.get());

    //パイロット設定
    pilot_ = pilot;
}

void Vitesse::UpdateInputMove()
{
    if (!camera_->GetIsMainCamera())return;

    //飛行中はY軸にも動くようにする
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

//ターゲットとなるコライダーを補足する
void Vitesse::TargetAcquisition()
{
    //全ヒットコライダーと内積値とスクリーン座標から補足するターゲットを取得する
    //補足できるコライダーが複数ある場合はより近い方を補足
    const auto& colliderList = Engine::collisionManager_->GetHitColliderList();

    //現在ターゲットに設定されているアクターまでのスクリーン距離
    float nearestDistSq = FLT_MAX;

    //ターゲットを検知したかのフラグ
    bool findTarget = false;

    //ターゲットが変更フラグをリセットしておく
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
            
            //内積値が負のコライダーは確実にカメラから写らないので省く
            float dot = cameraForward.Dot(toCollider);
            if (dot < 0)continue;

            //スクリーン座標から画面中央からどの程度の位置にいるか検索
            Vector2 colliderScreenPos = camera_->WorldToScreenPosition(colPos);
            D3D11_VIEWPORT viewport;
            DXSystem::GetViewport(1, &viewport);
            Vector2 center = { viewport.Width / 2, viewport.Height / 2 };

            const Vector2 centerToCollider = colliderScreenPos - center;
            const float distSq = centerToCollider.LengthSquared();

            //距離判定
            if (distSq < lockRadius_ * lockRadius_)
            {
                findTarget = true;

                //ターゲットがすでに設定されているか
                if (lockonTarget_.lock())
                {
                    //現在のターゲットとの距離を比較、必要ならターゲット更新
                    if (nearestDistSq > distSq)
                    {
                        //ターゲットを設定
                        lockonTarget_ = col->GetActor();
                        nearestDistSq = distSq;

                        //ターゲット変更フラグを立てる
                        //changeLockonTarget_ = true;
                    }
                }
                else
                {
                    //ターゲットを設定
                    lockonTarget_ = col->GetActor();
                    nearestDistSq = distSq;

                    //ターゲット変更フラグを立てる
                    changeLockonTarget_ = true;
                }
            }
        }
    }

    //ターゲットが見つからなかった場合は既存のターゲットを更新する
    if (!findTarget)
    {
        if (const auto& actor = lockonTarget_.lock())
        {
            const Vector3 targetPos = actor->GetTransform()->GetPosition();
            const Vector3 pos = transform_->GetPosition();

            const Vector3 toCollider = Vector3::Normalize(targetPos - pos);
            const Vector3 cameraForward = Vector3::Normalize(camera_->GetFocus() - camera_->GetEye());

            //内積値が負ならリセットする
            float dot = cameraForward.Dot(toCollider);
            if (dot < 0)lockonTarget_.reset();
            else
            {
                //スクリーン座標から画面中央からどの程度の位置にいるか検索
                Vector2 targetScreenPos = camera_->WorldToScreenPosition(targetPos);
                D3D11_VIEWPORT viewport;
                DXSystem::GetViewport(1, &viewport);
                Vector2 targetViewport = { targetScreenPos.x/viewport.Width, targetScreenPos.y/viewport.Height};
                
                //ビューポート座標から画面外に出ていないか調べる
                if ((targetViewport.x > 0 && targetViewport.x < 1) &&
                    (targetViewport.y > 0 && targetViewport.y < 1))
                {
                    //画面内にいるので引き続き補足
                }
                else
                {
                    //画面外に出ているのでリセット
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

    //ノックバック処理
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
    //入力値取得
    Vector2 input = Input::GameSupport::GetCameraRollVector();

    auto r = camera_->GetTransform()->GetRotation();
    const float rollSpeed = cameraRollSpeed_ * actor_->GetDeltaTime();
    r.x = r.x + input.y * rollSpeed;
    r.y = r.y + input.x * rollSpeed;
    camera_->GetTransform()->SetRotation(r);
}

void Vitesse::ThrusterUpdate()
{
    //エフェクト更新
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
