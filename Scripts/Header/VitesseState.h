#pragma once
#include "State.h"

class Vitesse;

namespace VitesseState
{
    //�n��ɂ�����ړ��X�e�[�g�i�ҋ@���܂܂�Ă���j
    class GMoveState : public State<Vitesse>
    {
    public:
        GMoveState(Vitesse* vitesse) : State(vitesse, "GMove") {}
        ~GMoveState() {}

        void Initialize() override;
        void Update(const float& elapsedTime) override;
        void Finalize() override;
    };

    //�󒆂ɂ�����ړ��X�e�[�g�i�ҋ@���܂܂�Ă���j
    class FMoveState : public State<Vitesse>
    {
    public:
        FMoveState(Vitesse* vitesse) : State(vitesse, "FMove") {}
        ~FMoveState() {}

        void Initialize() override;
        void Update(const float& elapsedTime) override;
        void Finalize() override;
    };

    //�����X�e�[�g
    class TakeOffState : public State<Vitesse>
    {
    public:
        TakeOffState(Vitesse* vitesse) : State(vitesse, "TakeOff") {}
        ~TakeOffState() {}

        void Initialize() override;
        void Update(const float& elapsedTime) override;
        void Finalize() override;
    };

    //�����X�e�[�g
    class LandingState : public State<Vitesse>
    {
    public:
        LandingState(Vitesse* vitesse) : State(vitesse, "Landing") {}
        ~LandingState() {}

        void Initialize() override;
        void Update(const float& elapsedTime) override;
        void Finalize() override;
    };

}

