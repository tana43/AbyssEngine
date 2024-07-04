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
    //�ʏ�̑��͏����ɉ����āA��s���[�h���̑��͏�����ǉ�����
    //�d�͂𖳎����A�J�����̌����Ă�������֓����悤�ɂ���

    if (flightMode_)
    {
        //��������
        {
            if (moveVec_.LengthSquared() > 0.01f)
            {
                velocity_ = velocity_ + moveVec_ * (acceleration_ * Time::deltaTime_);

                //���x����
                Vector2 velocityXZ = { velocity_.x,velocity_.z };
                if (velocityXZ.Length() > Max_Horizontal_Speed)
                {
                    velocityXZ.Normalize();
                    velocityXZ = velocityXZ * Max_Horizontal_Speed;

                    velocity_.x = velocityXZ.x;
                    velocity_.z = velocityXZ.y;
                }
                //�c�����ɂ����x����
                velocity_.y = std::clamp(velocity_.y, -Max_Vertical_Speed, Max_Vertical_Speed);
            }
            else//��������
            {
                //���͒l���قڂȂ��ꍇ�͌�������
                Vector3 veloNormal;
                velocity_.Normalize(veloNormal);

                Vector3 deceVelocity = velocity_ - (veloNormal * (deceleration_ * Time::deltaTime_));
                Vector3 deceVeloNormal;
                deceVelocity.Normalize(deceVeloNormal);

                //���Ε����̃x�N�g���ɂȂ��Ă��܂������x���x������Ȃ�A���x�����S�ɂO�ɂ���
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
    _ASSERT_EXPR(flightMode_, "���ݔ�s���[�h�ł͂���܂���");
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



