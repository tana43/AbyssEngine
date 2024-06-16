#include "Player.h"
#include "Engine.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SkeletalMesh.h"
#include "Camera.h"
#include "Actor.h"
#include "Input.h"
#include "MathHelper.h"

#include "Animator.h"
#include "Animation.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;

void Player::Initialize(const std::shared_ptr<Actor>& actor)
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
    runMoveAnimation_ = model_->GetAnimator()->AppendAnimation(moveAnim);

    model_->GetAnimator()->PlayAnimation(static_cast<int>(AnimState::Move));

    //プレイヤーカメラ設定(プレイヤーと親子関係に)
    //今はそのままアタッチしているが、後々独自のカメラ挙動をつくる
    const auto& c = Engine::sceneManager_->GetActiveScene().InstanceActor("Player Camera");
    camera_ = c->AddComponent<Camera>();
    //c->SetParent(actor_);
    camera_->fov_ = DirectX::XMConvertToRadians(80.0f);
    camera_->targetOffset_ = Vector3(0.8f, 1.4f, 0);
    camera_->SetViewTarget(transform_.get());
    camera_->SetEnableDebugController(false);
}

void Player::Update()
{
    //プレイヤーカメラがメインになっていなければ更新しない
    if (!camera_->GetIsMainCamera())return;

    MoveUpdate();

    CameraRollUpdate();
}

bool Player::DrawImGui()
{
    if (ImGui::TreeNode("Player"))
    {
        Character::DrawImGui();

        ImGui::DragFloat("Base Acceleration", &baseAcceleration_, 0.1f, 0.0f, 100.0f);

        ImGui::TreePop();
    }

    return false;
}

void Player::MoveUpdate()
{
    //入力されたベクトルから移動ベクトル取得
    auto inputVec = Input::GetMoveVector();

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

            //速度制限
            if (velocity_.Length() > Max_Speed)
            {
                velocity_.Normalize();
                velocity_ =  velocity_ * Max_Speed;
            }
        }
    }
    else
    {
        //入力値がほぼない場合は減速処理
        velocity_ = velocity_ - (velocity_ * (deceleration_ * Time::deltaTime_));

        //速度が０に近いときは完全に０にする
        if (velocity_.LengthSquared() > 0.01f)
        {
            velocity_ = {};
        }
    }

    //座標更新
    {
        auto pos = transform_->GetPosition();
        pos = pos + velocity_ * Time::deltaTime_;
        transform_->SetPosition(pos);
    }

    //回転
    TurnY(velocity_);

    //走っているか
    Max_Speed = Input::GetDashButton() ? Run_Max_Speed : Walk_Max_Speed;

    runMoveAnimation_->SetBlendWeight(velocity_.Length() / Run_Max_Speed);

    moveVec_ = {};
}

void Player::CameraRollUpdate()
{
    //入力値取得
    Vector2 input = Input::GetCameraRollVector();

    auto r = camera_->GetTransform()->GetRotation();
    const float rollSpeed = cameraRollSpeed_ * Time::deltaTime_;
    r.x = r.x + input.y * rollSpeed;
    r.y = r.y + input.x * rollSpeed;
    camera_->GetTransform()->SetRotation(r);
}
