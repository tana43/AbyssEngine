#include "Vitesse.h"
#include "Actor.h"
#include "Animator.h"
#include "SceneManager.h"
#include "Engine.h"
#include "Input.h"
#include "VitesseState.h"
#include "RenderManager.h"

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

    //地上移動
    AnimBlendSpace2D rMoveAnim = AnimBlendSpace2D(model_.get(), "RunMove", static_cast<int>(AnimState::Stand),Vector2(0,0));
    //前、右、左、後の順に追加
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Run_F), Vector2(0, 1));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Run_R), Vector2(1, 0));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Run_L), Vector2(-1, 0));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Run_B), Vector2(0, -1));
    groundMoveAnimation_ = model_->GetAnimator()->AppendAnimation(rMoveAnim);

    //空中移動
    AnimBlendSpace2D fMoveAnim = AnimBlendSpace2D(model_.get(), "FlyMove", static_cast<int>(AnimState::Fly_Idle), Vector2(0, 0));
    fMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Fly_F), Vector2(0, 1));
    fMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Fly_R), Vector2(1, 0));
    fMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Fly_L), Vector2(-1, 0));
    fMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Fly_B), Vector2(0, -1));
    flyMoveAnimation_ = model_->GetAnimator()->AppendAnimation(fMoveAnim);

    model_->GetAnimator()->PlayAnimation(static_cast<int>(AnimState::Run_Move));

    //プレイヤーカメラ設定(プレイヤーと親子関係に)
    //今はそのままアタッチしているが、後々独自のカメラ挙動をつくる
    const auto& c = Engine::sceneManager_->GetActiveScene().InstanceActor("Vitesse Camera");
    camera_ = c->AddComponent<Camera>();
    //c->SetParent(actor_);
    camera_->fov_ = DirectX::XMConvertToRadians(80.0f);
    camera_->targetOffset_ = Vector3(0.8f, 1.4f, 0);
    camera_->SetViewTarget(transform_.get());
    camera_->SetEnableDebugController(false);

    //トランスフォーム設定
    /*transform_->SetScaleFactor(7.0f);
    transform_->SetPosition(Vector3(0, 3.5f, 4.0f));*/

    //カメラ設定
    camera_->armLength_ = 18.0f;
    camera_->targetOffset_ = { 12.2f,14.5f,0 };

    //ステートマシン設定
    stateMachine_ = std::make_unique<StateMachine<State<Vitesse>>>();
    stateMachine_->RegisterState(new VitesseState::GroundMove(this));
    stateMachine_->RegisterState(new VitesseState::FlyMove(this));
    stateMachine_->RegisterState(new VitesseState::TakeOff(this));
    stateMachine_->RegisterState(new VitesseState::Landing(this));
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

    Engine::renderManager_->ChangeMainCamera(camera_.get());
}

void Vitesse::Update()
{
    stateMachine_->Update();

    HumanoidWeapon::Update();
}

void Vitesse::Move()
{
    HumanoidWeapon::Move();

    //上昇
    if (flightMode_ && Input::GameSupport::GetClimdButton())
    {
        Climb(climbSpeed_ * Time::deltaTime_);
    }


//#if 1
    //ブレンドアニメーションのWeight更新
    Vector3 velocityXZ = {velocity_.x,0,velocity_.z};
    if (fabsf(velocityXZ.LengthSquared()) < 0.01f)
    {
        groundMoveAnimation_->SetBlendWeight(Vector2(0,0));
        flyMoveAnimation_->SetBlendWeight(Vector2(0,0));
    }
    else
    {
        //前方向と進行方向の差のベクトルを算出
        const auto& forward = transform_->GetForward();
        //const auto& right = transform_->GetRight();
        Vector3 moveDirection;
        velocityXZ.Normalize(moveDirection);

        Vector2 result;

        //result.x = (forward.x * moveDirection.z + right.x * moveDirection.x);
        //result.y = forward.z * moveDirection.z + right.z * moveDirection.x;
        //result.Normalize();
        //result = result * (velocity_.Length() / Max_Speed);

        //内積による計算
        float dot = acosf(forward.Dot(moveDirection));
        float crossY = forward.z * moveDirection.x - forward.x * moveDirection.z;

        //左右判定
        //内積値が１のときにそのまま正負をひっくり返してしまうと大きく角度が変わってしまうので、それも考慮して計算する
        if (crossY < 0)dot = DirectX::XM_2PI - dot;
        result = { sinf(dot),cosf(dot) };
        result = result * (velocityXZ.Length() / Max_Horizontal_Speed);
        
        groundMoveAnimation_->SetBlendWeight(result);
        flyMoveAnimation_->SetBlendWeight(result);

        //移動方向に代入
        moveDirection_ = { result.x,0,result.y };
    }
//#else
//    runMoveAnimation_->SetBlendWeight((velocity_.Length() / Max_Speed) * 2);
//#endif // 1
//    flyMoveAnimation_->SetBlendWeight((velocity_.Length() / Max_Horizontal_Speed) * 2);

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

void Vitesse::UpdateInputMove()
{
    if (!camera_->isMainCamera_)return;

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
