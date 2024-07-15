#include "PlayerSoldier.h"
#include "Engine.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SkeletalMesh.h"
#include "StaticMesh.h"
#include "Camera.h"
#include "Actor.h"
#include "Input.h"
#include "MathHelper.h"

#include "Animator.h"
#include "Animation.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;

void Soldier::Initialize(const std::shared_ptr<Actor>& actor)
{
    //アクターとトランスフォームの登録
    Character::Initialize(actor);

    //モデル読み込み
    model_ = actor_->AddComponent<SkeletalMesh>("./Assets/Models/Soldier/Sci_Fi_Soldier_03_Idle.glb");
    model_->GetAnimator()->AppendAnimations({
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_WalkFwd.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_RunFwd.glb"
                },
        {
            "Walk","Run"
        });
    AnimBlendSpace1D moveAnim = AnimBlendSpace1D(model_.get(), "Move", 0, 2);
    moveAnim.AddBlendAnimation(1, 0.6f);
    moveAnimation_ = model_->GetAnimator()->AppendAnimation(moveAnim);

    model_->GetAnimator()->PlayAnimation(static_cast<int>(AnimState::Move));

    //武器を装備させる
    weaponModel_ = actor_->AddComponent<StaticMesh>("./Assets/Models/Soldier/Soldier_Gun.glb");
    model_->SocketAttach(weaponModel_, "middle_metacarpal_l");
    weaponModel_->GetSocketData().location_ = { 3.35f,-4.75f,6.3f };
    weaponModel_->GetSocketData().rotation_ = { 2.15f,168.7f,-99.25f };

    //プレイヤーカメラ設定(プレイヤーと親子関係に)
    //今はそのままアタッチしているが、後々独自のカメラ挙動をつくる
    const auto& c = Engine::sceneManager_->GetActiveScene().InstanceActor("Player Camera");
    camera_ = c->AddComponent<Camera>();
    //c->SetParent(actor_);
    camera_->fov_ = DirectX::XMConvertToRadians(80.0f);
    camera_->targetOffset_ = Vector3(0.4f, 0.6f, 0);
    camera_->armLength_ = 0.4f;
    camera_->cameraLagSpeed_ = 0.05f;
    camera_->SetViewTarget(transform_.get());
    camera_->SetEnableDebugController(false);
}

void Soldier::Update()
{
    //プレイヤーカメラがメインになっていなければ更新しない
    if (!camera_->GetIsMainCamera())return;

    MoveUpdate();

    CameraRollUpdate();
}

bool Soldier::DrawImGui()
{
    if (ImGui::TreeNode("Player"))
    {
        Character::DrawImGui();

        ImGui::DragFloat("Base Acceleration", &baseAcceleration_, 0.1f, 0.0f, 100.0f);

        ImGui::DragFloat("Jump Power", &jumpPower_, 0.02f, 0.01f, 100.0f);

        ImGui::TreePop();
    }

    return false;
}

void Soldier::MoveUpdate()
{
    //入力されたベクトルから移動ベクトル取得
    auto inputVec = Input::GameSupport::GetMoveVector();

    //入力値がある場合は加速
    if (inputVec.LengthSquared() > 0.01f)
    {
        moveVec_ = camera_->ConvertTo2DVectorFromCamera(inputVec);

        //スティックを少しだけ倒しているなら、加速も緩く、遅い速度を保たせる
        ///速力更新
        {
            //スティックの入力の大きさに応じて加速度を変更
            acceleration_ = baseAcceleration_ * inputVec.Length();

            velocity_ = velocity_ + moveVec_ * (acceleration_ * Time::deltaTime_);

        }
    }
    else
    {
        moveVec_ = {};
    }

    //Character::UpdateVelocity();

    //地形判定も含めた移動処理
    Character::UpdateMove();

    //回転
    //TurnY(velocity_);

    //ジャンプ
    if (Input::GameSupport::GetJumpButton())
    {
        Jump(jumpPower_);
    }

    //走っているか
    if (onGround_)
    {
        Max_Horizontal_Speed = Input::GameSupport::GetDashButton() ? Run_Max_Speed : Walk_Max_Speed;
    }

    const Vector2 veloXZ = { velocity_.x,velocity_.z };
    moveAnimation_->SetBlendWeight(veloXZ.Length() / Run_Max_Speed);

    moveVec_ = {};
}

void Soldier::CameraRollUpdate()
{
    //入力値取得
    Vector2 input = Input::GameSupport::GetCameraRollVector();

    auto r = camera_->GetTransform()->GetRotation();
    const float rollSpeed = cameraRollSpeed_ * Time::deltaTime_;
    r.x = r.x + input.y * rollSpeed;
    r.y = r.y + input.x * rollSpeed;
    camera_->GetTransform()->SetRotation(r);
}

void Soldier::BoardingTheVitesse()
{

}
