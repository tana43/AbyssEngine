#pragma once
#include "Character.h"
#include "MathHelper.h"

//���{�b�g�̊��N���X
class HumanoidWeapon : public AbyssEngine::Character
{
public:
    HumanoidWeapon();
    ~HumanoidWeapon() = default;

    void Update()override;
protected:
    void UpdateVelocity();

    void UpadteInputMove();

    bool flying = false;//��s����

};

