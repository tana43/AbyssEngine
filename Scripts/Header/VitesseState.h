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
        void Update() override;
        void Finalize() override;
    };

    //Game:Vitesse �󒆈ړ��X�e�[�g
    //�󒆂ɂ�����󒆈ړ��X�e�[�g�i�ҋ@���܂܂�Ă���j
    class FlyMove : Inheritance
    {
    public:
        FlyMove(Vitesse* vitesse) : State(vitesse, "FlyMove") {}
        ~FlyMove() {}

        void Initialize() override;
        void Update() override;
        void Finalize() override;
    };

    //Game:Vitesse �����X�e�[�g
    //�����X�e�[�g
    class TakeOff : Inheritance
    {
    public:
        TakeOff(Vitesse* vitesse) : State(vitesse, "TakeOff") {}
        ~TakeOff() {}

        void Initialize() override;
        void Update() override;
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
        void Update() override;
        void Finalize() override;

    private:
        //�������x
        float deceleration_ = 30.0f;

        //���̌������x���L���b�V��
        float cachedDeceleration_;
    };

}

