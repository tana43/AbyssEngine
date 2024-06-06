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
    //速力更新
    if (moveVec_.LengthSquared() < 0.01f)
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

void HumanoidWeapon::UpadteInputMove()
{
}

