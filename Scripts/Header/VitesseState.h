#pragma once
#include "State.h"
#include "MathHelper.h"

class Vitesse;

namespace VitesseState
{
    //Game:Vitesse �ړ��X�e�[�g
    //�n��ɂ�����ړ��X�e�[�g�i�ҋ@���܂܂�Ă���j
    class GroundMove : public State<Vitesse>
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
    class FlyMove : public State<Vitesse>
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
    class TakeOff : public State<Vitesse>
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
    class Landing : public State<Vitesse>
    {
    public:
        Landing(Vitesse* vitesse) : State(vitesse, "Landing") {}
        ~Landing() {}

        void Initialize() override;
        void Update() override;
        void Finalize() override;
    };

}

