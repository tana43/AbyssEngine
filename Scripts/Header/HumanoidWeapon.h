#pragma once
#include "Human.h"
#include "MathHelper.h"

//���{�b�g�̊��N���X
class HumanoidWeapon : public Human
{
public:
    HumanoidWeapon();
    ~HumanoidWeapon() = default;

    void Update()override;

    //���͏���
    void UpdateVelocity()override;

    void DrawImGui()override;

    virtual void ToFlightMode();//�t���C�g���[�h�ֈڍs
    virtual void ToGroundMode();//���ヂ�[�h�ֈڍs

public:
    const float& GetDefaultMaxHorizontalSpeed() const { return defaultMaxHorizontalSpeed; }
    const float& GetDefaultMaxVerticalSpeed() const { return defaultMaxVerticalSpeed; }

protected:
    virtual void Climb(float amount);//�㏸


    virtual void UpdateInputMove() {}

    bool flightMode_ = false;//��s���[�h��
    float inputAscent_ = 0.0f; //�㏸���͒l

    float fallSpeed_;//�������x(���x�������ʂŗp�ӂ���Ă���)
    float Fall_Max_Speed;//�d�͂ɂ���ĉ��Z����鑬�x�̏��

    float propulsion_ = 20.0f;//���i��(�ʏ�20���x)
    float climbSpeed_ = 50.0f;//�㏸���x

    float defaultMaxHorizontalSpeed = 40.0f;
    float defaultMaxVerticalSpeed = 40.0f;

    //����������
    bool slowDown_ = false;
};

