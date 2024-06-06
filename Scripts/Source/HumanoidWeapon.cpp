#include "HumanoidWeapon.h"
#include "Engine.h"

using namespace AbyssEngine;

HumanoidWeapon::HumanoidWeapon()
{
    Max_Speed = 1000.0f;
    acceleration_ = 5.0f;
    deceleration_ = 10.0f;
}

void HumanoidWeapon::Update()
{
    Character::UpdateMove();
}

void HumanoidWeapon::UpdateVelocity()
{
    //���͍X�V
    if (moveVec_.LengthSquared() < 0.01f)
    {
        velocity_ = velocity_ + moveVec_ * (acceleration_ * Time::deltaTime_);

        //���x����
        if (velocity_.Length() > Max_Speed)
        {
            velocity_.Normalize();
            velocity_ = velocity_ * Max_Speed;
        }
    }
    else//��������
    {
        //���͒l���قڂȂ��ꍇ�͌�������
        velocity_ = velocity_ - (velocity_ * (deceleration_ * Time::deltaTime_));

        //���x���O�ɋ߂��Ƃ��͊��S�ɂO�ɂ���
        if (velocity_.LengthSquared() > 0.01f)
        {
            velocity_ = {};
        }
    }


}

void HumanoidWeapon::UpadteInputMove()
{
}

