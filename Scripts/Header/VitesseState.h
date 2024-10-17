#pragma once
#include "State.h"
#include "MathHelper.h"

class Vitesse;

//�p����
#define Inheritance  public State<Vitesse>

namespace VitesseState
{
    //Game:Vitesse �ړ��X�e�[�g
    //�n��ɂ�����ړ��X�e�[�g�i�ҋ@���܂܂�Ă���j
    class GroundMove : Inheritance
    {
    public:
        GroundMove(Vitesse* vitesse) : State(vitesse, "GroundMove") {}
        ~GroundMove() {}

        void Initialize() override;
        void Update(float deltaTime) override;
        void Finalize() override;
    };

    //Game:Vitesse �󒆈ړ��X�e�[�g
    //�󒆂ɂ�����󒆈ړ��X�e�[�g�i�ҋ@���܂܂�Ă���j
    class Flight : Inheritance
    {
    public:
        Flight(Vitesse* vitesse) : State(vitesse, "Flight") {}
        ~Flight() {}

        void Initialize() override;
        void Update(float deltaTime) override;
        void Finalize() override;
    };

    //Game:Vitesse ����A������s�X�e�[�g
    //����A�����ړ��X�e�[�g
    class HighSpeedFlight : Inheritance
    {
    public:
        HighSpeedFlight(Vitesse* vitesse) : State(vitesse, "HighSpeedFlight") {}
        ~HighSpeedFlight() {}

        void Initialize() override;
        void Update(float deltaTime) override;
        void Finalize() override;

    private:
        float dodgeTime_ = 0.3f;
        float dodgeCameraArmLength_ = 40.0f;
        float highSpeedCameraArmLength_ = 26.0f;
        float cameraLagSpeed = 35.0f;

        //�΂߉�������Ă��邩
        bool rollingDodge_ = false;
    };

    //Game:Vitesse �����X�e�[�g
    //�����X�e�[�g
    class TakeOff : Inheritance
    {
    public:
        TakeOff(Vitesse* vitesse) : State(vitesse, "TakeOff") {}
        ~TakeOff() {}

        void Initialize() override;
        void Update(float deltaTime) override;
        void Finalize() override;

        inline static float altitude_ = 20.0f;//��������ۂ̏㏸���x
        inline static float requidTime_ = 0.6f;//��������ۂ̏��v����
    private:
        float startPosition_;
        float goalAltitude_;//�ڕW���x
    };

    //Game:Vitesse �����X�e�[�g
    //�����X�e�[�g
    class Landing : Inheritance
    {
    public:
        Landing(Vitesse* vitesse) : State(vitesse, "Landing") {}
        ~Landing() {}

        void Initialize() override;
        void Update(float deltaTime) override;
        void Finalize() override;

    private:
        //�������x
        float deceleration_ = 100.0f;

        //���̌������x���L���b�V��
        float cachedDeceleration_;
    };

    //�p�C���b�g��荞�݃X�e�[�g
    class Boarding : Inheritance
    {
    public:
        Boarding(Vitesse* vitesse) : State(vitesse, "Boarding") {}
        ~Boarding() {}

        void Initialize() override;
        void Update(float deltaTime) override;
        void Finalize() override;
    private:

        bool board_;//��荞�񂾂��ǂ���

    };

    //�ߐڍU�����d�|���ɂ����Ƃ��̃_�b�V��
    class MeleeAttackDash : Inheritance
    {
    public:
        MeleeAttackDash(Vitesse* vitesse) : State(vitesse, "MeleeAttackDash") {}
        ~MeleeAttackDash() {}

        void Initialize() override;
        void Update(float deltaTime) override;
        void Finalize() override;

    private:
        float cameraArmLength_ = 35.0f;
        float cameraLagSpeed_ = 30.0f;

        int step_ = 0;
    };

    //�ߐڍU��
    class MeleeAttack : Inheritance
    {
    public:
        MeleeAttack(Vitesse* vitesse) : State(vitesse, "MeleeAttack") {}
        ~MeleeAttack() {}

        void Initialize() override;
        void Update(float deltaTime) override;
        void Finalize() override;

    };
}

