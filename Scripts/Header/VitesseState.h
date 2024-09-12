#pragma once
#include "State.h"
#include "MathHelper.h"

class Vitesse;

//継承元
#define Inheritance  public State<Vitesse>

namespace VitesseState
{
    //Game:Vitesse 移動ステート
    //地上における移動ステート（待機も含まれている）
    class GroundMove : Inheritance
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
    class FlyMove : Inheritance
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
    class TakeOff : Inheritance
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
    class Landing : Inheritance
    {
    public:
        Landing(Vitesse* vitesse) : State(vitesse, "Landing") {}
        ~Landing() {}

        void Initialize() override;
        void Update() override;
        void Finalize() override;

    private:
        //減速速度
        float deceleration_ = 30.0f;

        //元の減速速度をキャッシュ
        float cachedDeceleration_;
    };

}

