#pragma once
#include "State.h"
#include "MathHelper.h"

class Vitesse;

namespace VitesseState
{
    //Game:Vitesse 移動ステート
    //地上における移動ステート（待機も含まれている）
    class GroundMove : public State<Vitesse>
    {
    public:
        GroundMove(Vitesse* vitesse) : State(vitesse, "GroundMove") {}
        ~GroundMove() {}

        void Initialize() override;
        void Update() override;
        void Finalize() override;
    };

    //Game:Vitesse 空中移動ステート
    //空中における空中移動ステート（待機も含まれている）
    class FlyMove : public State<Vitesse>
    {
    public:
        FlyMove(Vitesse* vitesse) : State(vitesse, "FlyMove") {}
        ~FlyMove() {}

        void Initialize() override;
        void Update() override;
        void Finalize() override;
    };

    //Game:Vitesse 離陸ステート
    //離陸ステート
    class TakeOff : public State<Vitesse>
    {
    public:
        TakeOff(Vitesse* vitesse) : State(vitesse, "TakeOff") {}
        ~TakeOff() {}

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

