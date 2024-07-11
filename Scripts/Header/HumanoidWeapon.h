#pragma once
#include "Character.h"
#include "MathHelper.h"

//ロボットの基底クラス
class HumanoidWeapon : public AbyssEngine::Character
{
public:
    HumanoidWeapon();
    ~HumanoidWeapon() = default;

    void Update()override;

    //速力処理
    void UpdateVelocity()override;

    bool DrawImGui()override;

    virtual void ToFlightMode();//フライトモードへ移行
    virtual void ToLandMode();//陸上モードへ移行

protected:
    virtual void Move()override;
    virtual void Climb(float amount);//上昇


    virtual void UpdateInputMove() {}

    bool flightMode_ = false;//飛行モードか
    float inputAscent_ = 0.0f; //上昇入力値

    float fallSpeed_;//落下速度(速度制限が別で用意されている)
    float Fall_Max_Speed;//重力によって加算される速度の上限

    float propulsion_ = 20.0f;//推進力(通常20程度)
    float climbSpeed_ = 50.0f;//上昇速度

    //減速したか
    bool slowDown_ = false;
};

