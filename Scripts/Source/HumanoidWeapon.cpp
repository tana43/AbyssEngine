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
    slowDown_ = false;

    //通常の速力処理に加えて、飛行モード時の速力処理を追加する
    //重力を無視し、カメラの向いている方向へ動くようにする

    if (flightMode_)
    {
        //水平方向
        {
            Vector3 moveVecXZ = { moveVec_.x,0.0f,moveVec_.z };
            if (moveVecXZ.LengthSquared() > 0.01f)
            {
                velocity_ = velocity_ + moveVecXZ * (acceleration_ * Time::deltaTime_);

                //速度制限
                Vector2 velocityXZ = { velocity_.x,velocity_.z };
                if (velocityXZ.Length() > Max_Horizontal_Speed)
                {
                    velocityXZ.Normalize();
                    velocityXZ = velocityXZ * Max_Horizontal_Speed;

                    velocity_.x = velocityXZ.x;
                    velocity_.z = velocityXZ.y;
                }
            }
            else//水平方向減速処理
            {
                //入力値がほぼない場合は減速処理
                Vector3 veloXZNormal = {velocity_.x,0.0f,velocity_.z};
                veloXZNormal.Normalize();

                Vector3 deceVelocityXZ = velocity_ - (veloXZNormal * (deceleration_ * Time::deltaTime_));
                deceVelocityXZ.y = 0;
                Vector3 deceVeloNormal;
                deceVelocityXZ.Normalize(deceVeloNormal);

                //反対方向のベクトルになってしまうか速度が遅すぎるなら、速度を完全に０にする
                if (veloXZNormal.Dot(deceVeloNormal) < 0 || deceVelocityXZ.LengthSquared() < 0.1f)
                {
                    velocity_.x = velocity_.z = {};
                }
                else
                {
                    velocity_.x = deceVelocityXZ.x;
                    velocity_.z = deceVelocityXZ.z;

                    //減速したのでフラグを立てる
                    slowDown_ = true;
                }
            }
        }

        //縦方向
        {
            if (fabsf(moveVec_.y) > 0.01f)
            {
                //加速
                velocity_.y += moveVec_.y * (acceleration_ * Time::deltaTime_);

                //速度制限
                velocity_.y = std::clamp(velocity_.y, -Max_Vertical_Speed, Max_Vertical_Speed);
            }
            else
            {
                //入力値がほとんどないので減速処理
                float direY = velocity_.y > 0.0f ? 1.0f : -1.0f;

                //減速
                velocity_.y -= (direY * deceleration_ * Time::deltaTime_);

                //減速制限
                 //反対方向のベクトルになってしまうか速度が遅すぎるなら、速度を完全に０にする
                if (direY * velocity_.y < 0.0f || fabsf(velocity_.y) < 0.1f)
                {
                    velocity_.y = 0;
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



