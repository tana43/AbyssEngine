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
    //moveVecは飛行中以外でY軸に値が入ることはない

    //垂直移動処理
    //if (flyingMode_)
    //{
    //    //飛行中
    //    
    //    //落下速度を徐々に小さくする
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
    //    //落下速度の更新
    //    fallSpeed_ -= Gravity * Time::deltaTime_;
    //    if (fallSpeed_ > Fall_Max_Speed)
    //    {
    //        fallSpeed_ = Fall_Max_Speed;
    //    }
    //}

    //速力更新
    if (moveVec_.LengthSquared() > 0.01f)
    {
        velocity_ = velocity_ + moveVec_ * (acceleration_ * propulsion_ * Time::deltaTime_);

        //速度制限
        if (velocity_.Length() > Max_Horizontal_Speed)
        {
            velocity_.Normalize();
            velocity_ = velocity_ * Max_Horizontal_Speed;
        }
    }
    else//減速処理
    {
        //if (flyingMode_)
        //{

        //}
        //else
        //{

        //}
        //入力値がほぼない場合は減速処理
        Vector3 veloNormal = velocity_;
        veloNormal.Normalize();
        velocity_ = velocity_ - (veloNormal * (deceleration_ * propulsion_ * Time::deltaTime_));

        //速度が０に近いときは完全に０にする
        if (velocity_.LengthSquared() < 0.1f)
        {
            velocity_ = {};
        }
    }

    
}



