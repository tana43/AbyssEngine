#include "Player.h"
#include "Engine.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SkeletalMesh.h"
#include "Camera.h"
#include "Actor.h"
#include "Input.h"
#include "MathHelper.h"
#include "imgui/imgui.h"

using namespace AbyssEngine;

void Player::Initialize(const std::shared_ptr<Actor>& actor)
{
    //アクターとトランスフォームの登録
    Character::Initialize(actor);

    transform_->SetPositionY(3.17f);

    //モデル読み込み
    model_ = actor_->AddComponent<SkeletalMesh>("./Assets/Models/UE/Manny/Manny_Idle.glb");
    model_->AppendAnimations({
                "./Assets/Models/UE/Manny/Manny_Walk.glb",
                "./Assets/Models/UE/Manny/Manny_Run.glb"
                });

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
    MoveUpdate();
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
        auto moveVec = camera_->ConvertTo2DVectorFromCamera(inputVec);

        //スティックを少しだけ倒しているなら、加速も緩く、遅い速度を保たせる
        ///速力更新
        {
            //スティックの入力の大きさに応じて加速度を変更
            acceleration_ = baseAcceleration_ * inputVec.Length();

            velocity_ = velocity_ + moveVec * (acceleration_ * Time::deltaTime_);

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

}
