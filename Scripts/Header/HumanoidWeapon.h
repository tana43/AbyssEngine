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

    //���͏���
    void UpdateVelocity()override;

    bool DrawImGui()override;

    virtual void ToFlightMode();//�t���C�g���[�h�ֈڍs
    virtual void ToLandMode();//���ヂ�[�h�ֈڍs

protected:
    virtual void Move()override;
    virtual void Climb(float amount);//�㏸


    virtual void UpdateInputMove() {}

    bool flightMode_ = false;//��s���[�h��
    float inputAscent_ = 0.0f; //�㏸���͒l

    float fallSpeed_;//�������x(���x�������ʂŗp�ӂ���Ă���)
    float Fall_Max_Speed;//�d�͂ɂ���ĉ��Z����鑬�x�̏��

    float propulsion_ = 20.0f;//���i��(�ʏ�20���x)
    float climbSpeed_ = 50.0f;//�㏸���x

    //����������
    bool slowDown_ = false;
};

