#pragma once
#include "State.h"
#include "MathHelper.h"

class Vitesse;

namespace VitesseState
{
    //Game:Vitesse �ړ��X�e�[�g
    //�n��ɂ�����ړ��X�e�[�g�i�ҋ@���܂܂�Ă���j
    class GMoveState : public State<Vitesse>
    {
    public:
        GMoveState(Vitesse* vitesse) : State(vitesse, "GMove") {}
        ~GMoveState() {}

        void Initialize() override;
        void Update() override;
        void Finalize() override;
    };

    //Game:Vitesse �󒆈ړ��X�e�[�g
    //�󒆂ɂ�����󒆈ړ��X�e�[�g�i�ҋ@���܂܂�Ă���j
    class FMoveState : public State<Vitesse>
    {
    public:
        FMoveState(Vitesse* vitesse) : State(vitesse, "FMove") {}
        ~FMoveState() {}

        void Initialize() override;
        void Update() override;
        void Finalize() override;

    private:

    };

    //Game:Vitesse �����X�e�[�g
    //�����X�e�[�g
    class TakeOffState : public State<Vitesse>
    {
    public:
        TakeOffState(Vitesse* vitesse) : State(vitesse, "TakeOff") {}
        ~TakeOffState() {}

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
    class LandingState : public State<Vitesse>
    {
    public:
        LandingState(Vitesse* vitesse) : State(vitesse, "Landing") {}
        ~LandingState() {}

        void Initialize() override;
        void Update() override;
        void Finalize() override;
    };

}

