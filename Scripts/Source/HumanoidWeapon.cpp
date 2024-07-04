#include "HumanoidWeapon.h"
#include "Engine.h"
#include "imgui/imgui.h"

#include <algorithm>

using namespace AbyssEngine;

HumanoidWeapon::HumanoidWeapon()
{
    Max_Horizontal_Speed = 40.0f;
    baseRotSpeed_ = 500.0f;
    acceleration_ = 30.0f;
    deceleration_ = 60.0f;
}

void HumanoidWeapon::Update()
{
    UpdateInputMove();
    Move();
}

void HumanoidWeapon::UpdateVelocity()
{
    //通常の速力処理に加えて、飛行モード時の速力処理を追加する
    //重力を無視し、カメラの向いている方向へ動くようにする

    if (flightMode_)
    {
        //水平方向
        {
            if (moveVec_.LengthSquared() > 0.01f)
            {
                velocity_ = velocity_ + moveVec_ * (acceleration_ * Time::deltaTime_);

                //速度制限
                Vector2 velocityXZ = { velocity_.x,velocity_.z };
                if (velocityXZ.Length() > Max_Horizontal_Speed)
                {
                    velocityXZ.Normalize();
                    velocityXZ = velocityXZ * Max_Horizontal_Speed;

                    velocity_.x = velocityXZ.x;
                    velocity_.z = velocityXZ.y;
                }
                //縦方向にも速度制限
                velocity_.y = std::clamp(velocity_.y, -Max_Vertical_Speed, Max_Vertical_Speed);
            }
            else//減速処理
            {
                //入力値がほぼない場合は減速処理
                Vector3 veloNormal;
                velocity_.Normalize(veloNormal);

                Vector3 deceVelocity = velocity_ - (veloNormal * (deceleration_ * Time::deltaTime_));
                Vector3 deceVeloNormal;
                deceVelocity.Normalize(deceVeloNormal);

                //反対方向のベクトルになってしまうか速度が遅すぎるなら、速度を完全に０にする
                if (veloNormal.Dot(deceVeloNormal) < 0 || deceVelocity.LengthSquared() < 0.1f)
                {
                    velocity_ = {};
                }
                else
                {
                    velocity_ = deceVelocity;
                }
            }
        }
    }
    else
    {
        Character::UpdateVelocity();
    }
}

bool HumanoidWeapon::DrawImGui()
{
    Character::DrawImGui();

    ImGui::SliderFloat("Climb", &climbSpeed_, 0.0f, 200.0f);

    return true;
}

void HumanoidWeapon::Move()
{
    Character::Move();
}

void HumanoidWeapon::Climb(float amount)
{
    _ASSERT_EXPR(flightMode_, "現在飛行モードではありません");
    if (!flightMode_)return;
    velocity_.y += amount;
}

void HumanoidWeapon::ToFlightMode()
{
    flightMode_ = true;
}

void HumanoidWeapon::ToLandMode()
{
    flightMode_ = false;
}



