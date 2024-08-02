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

#include "ThrusterEffect.h"

using namespace AbyssEngine;

Vitesse::Vitesse() : HumanoidWeapon()
{
}

void Vitesse::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    HumanoidWeapon::Initialize(actor);

    //モデル読み込み
    model_ = actor_->AddComponent<SkeletalMesh>("./Assets/Models/Vitesse/Vitesse_UE_01_Stand.glb");
    model_->GetAnimator()->AppendAnimations({
                "./Assets/Models/Vitesse/Vitesse_UE_01_Run_F.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Run_R.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Run_L.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Run_B.glb",
                //"./Assets/Models/Vitesse/Vitesse_UE_01_FrontFly.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_Idle.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_F.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_R.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_L.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_B.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_Up.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_Down.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Landing.glb"
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
            "Fly_Landing"
        });

    //ループ再生しないように
    model_->GetAnimator()->GetAnimations().at(static_cast<int>(AnimationIndex::Landing))->SetLoopFlag(false);

    //地上移動
    AnimBlendSpace2D rMoveAnim = AnimBlendSpace2D(model_.get(), "RunMove", static_cast<int>(AnimationIndex::Stand),Vector2(0,0));
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
    AnimBlendSpace1D fMoveAnim1D = AnimBlendSpace1D(model_.get(), "FlyMoveUpDown", static_cast<int>(AnimationIndex::Fly_Idle), static_cast<int>(AnimationIndex::Fly_Up));
    fMoveAnim1D.AddBlendAnimation(static_cast<int>(AnimationIndex::Fly_Down), -1.0f);
    auto* f1d = model_->GetAnimator()->AppendAnimation(fMoveAnim1D);
    f1d->SetMinWeight(-1.0f);

    AnimBlendSpace2D fMoveAnim2D = AnimBlendSpace2D(model_.get(), "FlyMove2D", static_cast<int>(AnimationIndex::Fly_Idle), Vector2(0, 0));
    fMoveAnim2D.AddBlendAnimation(static_cast<int>(AnimationIndex::Fly_F), Vector2(0, 1));
    fMoveAnim2D.AddBlendAnimation(static_cast<int>(AnimationIndex::Fly_R), Vector2(1, 0));
    fMoveAnim2D.AddBlendAnimation(static_cast<int>(AnimationIndex::Fly_L), Vector2(-1, 0));
    fMoveAnim2D.AddBlendAnimation(static_cast<int>(AnimationIndex::Fly_B), Vector2(0, -1));
    auto* f2d = model_->GetAnimator()->AppendAnimation(fMoveAnim2D);

    AnimBlendSpaceFlyMove fMoveAnim = AnimBlendSpaceFlyMove(model_.get(), "FlyMove3D",f2d,f1d);
    flyMoveAnimation_ = model_->GetAnimator()->AppendAnimation(fMoveAnim);
#endif // 0

    model_->GetAnimator()->PlayAnimation(static_cast<int>(AnimationIndex::Run_Move));

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

    //ステートマシン設定
    stateMachine_ = actor->AddComponent<StateMachine<State<Vitesse>>>();
    stateMachine_->RegisterState(new VitesseState::GroundMove(this));
    stateMachine_->RegisterState(new VitesseState::FlyMove(this));
    stateMachine_->RegisterState(new VitesseState::TakeOff(this));
    stateMachine_->RegisterState(new VitesseState::Landing(this));

    //アニメーションステートマシーン設定
    animStateMachine_ = actor->AddComponent<StateMachine<State<Animator>>>();
    animStateMachine_->RegisterState(new VitesseAnimState::AnimGroundMove(model_->GetAnimator().get()));
    animStateMachine_->RegisterState(new VitesseAnimState::AnimFlyMove(model_->GetAnimator().get()));

    //初期ステート設定
    animStateMachine_->SetState(static_cast<int>(AnimationState::Ground_Move));
    stateMachine_->SetState(static_cast<int>(ActionState::GMove));

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

}

void Vitesse::Update()
{
    const auto& pilot = pilot_.lock();
    
    //パイロットが搭乗しているか
    if (pilot && pilot->GetVitesseOnBoard())
    {
        stateMachine_->SetActive(true);
    }
    else
    {
        stateMachine_->SetActive(false);
    }

    HumanoidWeapon::Update();

    //上昇
    if (flightMode_ && Input::GameSupport::GetClimdButton())
    {
        Climb(climbSpeed_ * Time::deltaTime_);
    }

    animStateMachine_->Update();

    CameraRollUpdate();

    transform_->CalcWorldMatrix();

    ThrusterUpdate();
}

bool Vitesse::DrawImGui()
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

    return true;
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

void Vitesse::ChangeState(const ActionState& state)
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
        moveVec_.y += Input::GameSupport::GetClimdButton() ? 1.0f : 0.0f;
    }
    else
    {
        moveVec_ = camera_->ConvertTo2DVectorFromCamera(Input::GameSupport::GetMoveVector());
    }
}

void Vitesse::CameraRollUpdate()
{
    //入力値取得
    Vector2 input = Input::GameSupport::GetCameraRollVector();

    auto r = camera_->GetTransform()->GetRotation();
    const float rollSpeed = cameraRollSpeed_ * Time::deltaTime_;
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
