#pragma once
#include "Character.h"

//ロボットの
class HumanoidWeapon : public AbyssEngine::Character
{
public:
    HumanoidWeapon() {}
    ~HumanoidWeapon() = default;

    void Update()override;
protected:
    void UpdateVelocity()

    bool flying = false;//飛行中か

};

