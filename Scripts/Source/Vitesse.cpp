#include "Vitesse.h"
#include "Actor.h"
#include "Animator.h"
#include "SceneManager.h"
#include "Engine.h"
#include "Input.h"

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
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_Up.glb"
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
            "Fly_Up"
        });

    //地上移動
    AnimBlendSpace2D rMoveAnim = AnimBlendSpace2D(model_.get(), "RunMove", static_cast<int>(AnimState::Stand),Vector2(0,0));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Stand), Vector2(90, 0));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Stand), Vector2(180, 0));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Stand), Vector2(-90, 0));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Stand), Vector2(-180, 0));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Run_F), Vector2(0, 1.0f));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Run_R), Vector2(90, 1.0f));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Run_L), Vector2(-90, 1.0f));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Run_B), Vector2(180, 1.0f));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Run_B), Vector2(-180, 1.0f));
    runMoveAnimation_ = model_->GetAnimator()->AppendAnimation(rMoveAnim);

    //空中移動
    AnimBlendSpace1D fmoveAnim = AnimBlendSpace1D(model_.get(), "FlyMove",
        static_cast<int>(AnimState::Fly_Idle), static_cast<int>(AnimState::Fly_F));
    flyMoveAnimation_ = model_->GetAnimator()->AppendAnimation(fmoveAnim);

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
}

void Vitesse::Move()
{
    HumanoidWeapon::Move();

    //現在向いている方向と速力の向きとの差
    
    runMoveAnimation_->SetBlendWeight(,(velocity_.Length() / Max_Speed) * 2);
    flyMoveAnimation_->SetBlendWeight((velocity_.Length() / Max_Speed) * 2);

    CameraRollUpdate();
}

void Vitesse::UpdateInputMove()
{
    if (!camera_->isMainCamera_)return;

    //飛行中はY軸にも動くようにする
    if (flyingMode_)
    {
        moveVec_ = camera_->ConvertTo3DVectorFromCamera(Input::GetMoveVector());
    }
    else
    {
        moveVec_ = camera_->ConvertTo2DVectorFromCamera(Input::GetMoveVector());
    }


}

void Vitesse::CameraRollUpdate()
{
    //入力値取得
    Vector2 input = Input::GetCameraRollVector();

    auto r = camera_->GetTransform()->GetRotation();
    const float rollSpeed = cameraRollSpeed_ * Time::deltaTime_;
    r.x = r.x + input.y * rollSpeed;
    r.y = r.y + input.x * rollSpeed;
    camera_->GetTransform()->SetRotation(r);
}