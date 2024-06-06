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
    void UpdateVelocity();

    void UpadteInputMove();

    bool flying = false;//飛行中か

};

