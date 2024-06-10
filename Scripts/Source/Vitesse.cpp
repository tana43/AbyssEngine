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
                "./Assets/Models/Vitesse/Vitesse_UE_01_FrontFly.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_RightFly.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_LeftFly.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_SkyIdle.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Jump.glb"
        },
        {
            "Walk","FrontFly","RightFly","LeftFly","SkyIdle","Jump"
        });
    AnimBlendSpace1D moveAnim = AnimBlendSpace1D(model_.get(), "Move", 
        static_cast<int>(AnimState::Idle), static_cast<int>(AnimState::FlyFront));
    moveAnimation_ = model_->GetAnimator()->AppendAnimation(moveAnim);

    model_->GetAnimator()->PlayAnimation(static_cast<int>(AnimState::Move));

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

    moveAnimation_->SetBlendWeight((velocity_.Length() / Max_Speed) * 2);
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
