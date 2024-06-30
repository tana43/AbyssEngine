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
protected:
    virtual void Move()override;

    virtual void UpdateInputMove() {}

    bool flyingMode_ = false;//飛行モードか
    float inputAscent_ = 0.0f; //上昇入力値

    float fallSpeed_;//落下速度(速度制限が別で用意されている)
    float Fall_Max_Speed;//重力によって加算される速度の上限

    float propulsion_ = 20.0f;//推進力(通常20程度)
};

