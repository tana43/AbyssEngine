#include "HumanoidWeapon.h"
#include "Engine.h"
#include "imgui/imgui.h"

#include <algorithm>

using namespace AbyssEngine;

HumanoidWeapon::HumanoidWeapon()
{
    Max_Horizontal_Speed = 40.0f;
    Max_Vertical_Speed = 40.0f;
    defaultMaxHorizontalSpeed = Max_Horizontal_Speed;
    defaultMaxVerticalSpeed = Max_Vertical_Speed;
    baseRotSpeed_ = 500.0f;
    acceleration_ = 60.0f;
    deceleration_ = 60.0f;
    speedingDecel_ = 200.0f;
    Gravity = -30.0f;
}

void HumanoidWeapon::Update()
{
    Character::Update();
}

void HumanoidWeapon::UpdateVelocity()
{
    slowDown_ = false;

    //�ʏ�̑��͏����ɉ����āA��s���[�h���̑��͏�����ǉ�����
    //�d�͂𖳎����A�J�����̌����Ă�������֓����悤�ɂ���

    if (flightMode_)
    {
        //��������
        {
            Vector3 moveVecXZ = { moveVec_.x,0.0f,moveVec_.z };
            if (moveVecXZ.LengthSquared() > 0.01f)
            {
                velocity_ = velocity_ + moveVecXZ * (acceleration_ * Time::deltaTime_);

                //���x����
                Vector2 velocityXZ = { velocity_.x,velocity_.z };
                float spd = velocityXZ.Length();
                if (spd > Max_Horizontal_Speed)
                {
                    velocityXZ.Normalize();
                    if (spd - Max_Horizontal_Speed < 0.1f)
                    {
                        //���̂܂܍ő呬�x����
                        velocityXZ = velocityXZ * Max_Horizontal_Speed;
                    }
                    else
                    {
                        //����������
                        float newSpeed = spd - speedingDecel_ * Time::deltaTime_;
                        velocityXZ = velocityXZ * newSpeed;
                    }

                    velocity_.x = velocityXZ.x;
                    velocity_.z = velocityXZ.y;
                }
            }
            else//����������������
            {
                //���͒l���قڂȂ��ꍇ�͌�������
                Vector3 veloXZNormal = {velocity_.x,0.0f,velocity_.z};
                veloXZNormal.Normalize();

                Vector3 deceVelocityXZ = velocity_ - (veloXZNormal * (deceleration_ * Time::deltaTime_));
                deceVelocityXZ.y = 0;
                Vector3 deceVeloNormal;
                deceVelocityXZ.Normalize(deceVeloNormal);

                //���Ε����̃x�N�g���ɂȂ��Ă��܂������x���x������Ȃ�A���x�����S�ɂO�ɂ���
                if (veloXZNormal.Dot(deceVeloNormal) < 0 || deceVelocityXZ.LengthSquared() < 0.1f)
                {
                    velocity_.x = velocity_.z = {};
                }
                else
                {
                    velocity_.x = deceVelocityXZ.x;
                    velocity_.z = deceVelocityXZ.z;

                    //���������̂Ńt���O�𗧂Ă�
                    slowDown_ = true;
                }
            }
        }

        //�c����
        {
            if (fabsf(moveVec_.y) > 0.01f)
            {
                //����
                velocity_.y += moveVec_.y * (acceleration_ * Time::deltaTime_);

                //���x����
                velocity_.y = std::clamp(velocity_.y, -Max_Vertical_Speed, Max_Vertical_Speed);
            }
            else
            {
                //���͒l���قƂ�ǂȂ��̂Ō�������
                float direY = velocity_.y > 0.0f ? 1.0f : -1.0f;

                //����
                velocity_.y -= (direY * deceleration_ * Time::deltaTime_);

                //��������
                 //���Ε����̃x�N�g���ɂȂ��Ă��܂������x���x������Ȃ�A���x�����S�ɂO�ɂ���
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

void HumanoidWeapon::DrawImGui()
{
    Character::DrawImGui();

    ImGui::SliderFloat("Climb", &riseSpeed_, 0.0f, 200.0f);

}

void HumanoidWeapon::Rise(float amount)
{
    _ASSERT_EXPR(flightMode_, "���ݔ�s���[�h�ł͂���܂���");
    if (!flightMode_)return;
    velocity_.y += amount;
}

void HumanoidWeapon::ToFlightMode()
{
    flightMode_ = true;
}

void HumanoidWeapon::ToGroundMode()
{
    flightMode_ = false;
}



