#pragma once
#include "State.h"
#include "MathHelper.h"

class Vitesse;

namespace VitesseState
{
    //Game:Vitesse 移動ステート
    //地上における移動ステート（待機も含まれている）
    class GMoveState : public State<Vitesse>
    {
    public:
        GMoveState(Vitesse* vitesse) : State(vitesse, "GMove") {}
        ~GMoveState() {}

        void Initialize() override;
        void Update() override;
        void Finalize() override;
    };

    //Game:Vitesse 空中移動ステート
    //空中における空中移動ステート（待機も含まれている）
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

    //Game:Vitesse 離陸ステート
    //離陸ステート
    class TakeOffState : public State<Vitesse>
    {
    public:
        TakeOffState(Vitesse* vitesse) : State(vitesse, "TakeOff") {}
        ~TakeOffState() {}

        void Initialize() override;
        void Update() override;
        void Finalize() override;

        inline static float altitude_ = 20.0f;//離陸する際の上昇高度
        inline static float requidTime_ = 0.6f;//離陸する際の所要時間
    private:
        float startPosition_;
        float goalAltitude_;//目標高度
    };

    //Game:Vitesse 着陸ステート
    //着陸ステート
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

