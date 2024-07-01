#pragma once
#include "State.h"

class Vitesse;

namespace VitesseState
{
    //地上における移動ステート（待機も含まれている）
    class GMoveState : public State<Vitesse>
    {
    public:
        GMoveState(Vitesse* vitesse) : State(vitesse, "GMove") {}
        ~GMoveState() {}

        void Initialize() override;
        void Update(const float& elapsedTime) override;
        void Finalize() override;
    };

    //空中における移動ステート（待機も含まれている）
    class FMoveState : public State<Vitesse>
    {
    public:
        FMoveState(Vitesse* vitesse) : State(vitesse, "FMove") {}
        ~FMoveState() {}

        void Initialize() override;
        void Update(const float& elapsedTime) override;
        void Finalize() override;
    };

    //離陸ステート
    class TakeOffState : public State<Vitesse>
    {
    public:
        TakeOffState(Vitesse* vitesse) : State(vitesse, "TakeOff") {}
        ~TakeOffState() {}

        void Initialize() override;
        void Update(const float& elapsedTime) override;
        void Finalize() override;
    };

    //着陸ステート
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

