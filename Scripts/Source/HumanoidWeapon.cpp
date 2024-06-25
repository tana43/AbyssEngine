#include "HumanoidWeapon.h"
#include "Engine.h"

using namespace AbyssEngine;

HumanoidWeapon::HumanoidWeapon()
{
    Max_Horizontal_Speed = 40.0f;
    deceleration_ = 1.5f;
}

void HumanoidWeapon::Update()
{
    UpdateInputMove();
    Move();
}

void HumanoidWeapon::Move()
{
    Character::Move();
}

void HumanoidWeapon::UpdateVelocity()
{
    //moveVec�͔�s���ȊO��Y���ɒl�����邱�Ƃ͂Ȃ�

    //�����ړ�����
    //if (flyingMode_)
    //{
    //    //��s��
    //    
    //    //�������x�����X�ɏ���������
    //    if (fallSpeed_ > 0.1f)
    //    {
    //        fallSpeed_ -= propulsion_ * moveVec_.y * Time::deltaTime_;
    //    }
    //    else
    //    {
    //        fallSpeed_ = 0.0f;
    //    }
    //}
    //else
    //{
    //    //�������x�̍X�V
    //    fallSpeed_ -= Gravity * Time::deltaTime_;
    //    if (fallSpeed_ > Fall_Max_Speed)
    //    {
    //        fallSpeed_ = Fall_Max_Speed;
    //    }
    //}

    //���͍X�V
    if (moveVec_.LengthSquared() > 0.01f)
    {
        velocity_ = velocity_ + moveVec_ * (acceleration_ * propulsion_ * Time::deltaTime_);

        //���x����
        if (velocity_.Length() > Max_Horizontal_Speed)
        {
            velocity_.Normalize();
            velocity_ = velocity_ * Max_Horizontal_Speed;
        }
    }
    else//��������
    {
        //if (flyingMode_)
        //{

        //}
        //else
        //{

        //}
        //���͒l���قڂȂ��ꍇ�͌�������
        Vector3 veloNormal = velocity_;
        veloNormal.Normalize();
        velocity_ = velocity_ - (veloNormal * (deceleration_ * propulsion_ * Time::deltaTime_));

        //���x���O�ɋ߂��Ƃ��͊��S�ɂO�ɂ���
        if (velocity_.LengthSquared() < 0.1f)
        {
            velocity_ = {};
        }
    }

    
}



