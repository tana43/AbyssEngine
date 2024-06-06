#include "CharacterManager.h"
#include "Engine.h"
#include "Actor.h"
#include <algorithm>

#include "imgui/imgui.h"

using namespace AbyssEngine;

void Character::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //マネージャーの登録と初期化
    Engine::characterManager_->AddCharacter(std::static_pointer_cast<Character>(shared_from_this()));
    actor_ = actor;
    transform_ = actor->GetTransform();
}

bool Character::DrawImGui()
{
    ImGui::DragFloat3("Velocity", &velocity_.x);

    float speed = velocity_.Length();
    ImGui::SliderFloat("Speed",&speed,0.0f,Max_Speed);

    ImGui::DragFloat("Max Speed", &Max_Speed,0.1f,0.1f);

    ImGui::SliderFloat("Accel", &acceleration_, 0.0f, 10.0f);
    ImGui::SliderFloat("Decel", &deceleration_, 0.0f, 10.0f);

    ImGui::SliderFloat("Rot Speed", &baseRotSpeed_, 0.0f, 1000.0f);
    ImGui::SliderFloat("Max Rot Speed", &Max_Rot_Speed,0.0f,1000.0f);
    ImGui::SliderFloat("Min Rot Speed", &Min_Rot_Speed,0.0f,1000.0f);

    return true;
}

void Character::TurnY(Vector3 dir, bool smooth)
{
    if (dir.LengthSquared() == 0)return;

    //XZ軸のみのベクトル正規化
    Vector2 d = { dir.x,dir.z };
    d.Normalize();
    dir = { d.x,0,d.y };
    auto forward = transform_->GetForward();

    float rotY = transform_->GetRotation().y;

    //内積値から最終的に向きたい角度を計算する
    float dot = forward.Dot(dir);
    if (dot > 0.99f)return;
    float rotAmount = acosf(dot);

    float rotSpeed = 0;

    //なめらかな振り向きか、即座に振り向くか
    if (smooth)
    {
        //内積値を0~1の範囲に
        dot = dot + 1.0f;

        rotSpeed = baseRotSpeed_ * rotAmount;
        
    }
    else
    {
        rotSpeed = rotAmount;
    }

    //回転速度制限
    std::clamp(rotSpeed, Min_Rot_Speed, Max_Rot_Speed);

    //外積のY軸のみ求め、回転方向を求める
    float crossY = forward.z * dir.x - forward.x * dir.z;
    if (crossY < 0) rotSpeed = -rotSpeed;
    
    rotY += rotSpeed * Time::deltaTime_;
    transform_->SetRotationY(rotY);
}

void Character::Move()
{
    UpdateVelocity();
    UpdateMove(); 
}

void Character::UpdateVelocity()
{
    //速力更新
    {
        if (moveVec_.LengthSquared() > 0.01f)
        {
            velocity_ = velocity_ + moveVec_ * (acceleration_ * Time::deltaTime_);

            //速度制限
            if (velocity_.Length() > Max_Speed)
            {
                velocity_.Normalize();
                velocity_ = velocity_ * Max_Speed;
            }
        }
        else//減速処理
        {
            //入力値がほぼない場合は減速処理
            velocity_ = velocity_ - (velocity_ * (deceleration_ * Time::deltaTime_));

            //速度が０に近いときは完全に０にする
            if (velocity_.LengthSquared() > 0.01f)
            {
                velocity_ = {};
            }
        }
    }
}

void Character::UpdateMove()
{
    //座標更新
    {
        auto pos = transform_->GetPosition();
        pos = pos + velocity_ * Time::deltaTime_;
        transform_->SetPosition(pos);
    }

    //回転
    TurnY(velocity_);
}
