#pragma once
#include "Character.h"

//���{�b�g��
class HumanoidWeapon : public AbyssEngine::Character
{
public:
    HumanoidWeapon() {}
    ~HumanoidWeapon() = default;

    void Update()override;
protected:
    void UpdateVelocity()

    bool flying = false;//��s����

};

