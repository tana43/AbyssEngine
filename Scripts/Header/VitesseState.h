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
    class Flight : Inheritance
    {
    public:
        Flight(Vitesse* vitesse) : State(vitesse, "Flight") {}
        ~Flight() {}

        void Initialize() override;
        void Update() override;
        void Finalize() override;
    };

    //Game:Vitesse 高速飛行ステート
    //高速移動ステート
    class HighSpeedFlight : Inheritance
    {
    public:
        HighSpeedFlight(Vitesse* vitesse) : State(vitesse, "HighSpeedFlight") {}
        ~HighSpeedFlight() {}

        void Initialize() override;
        void Update() override;
        void Finalize() override;

    private:
        float dodgeTime_ = 0.3f;
        float dodgeCameraArmLength_ = 40.0f;
        float highSpeedCameraArmLength_ = 26.0f;
        float cameraLagSpeed = 35.0f;

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

    //パイロット乗り込みステート
    class Boarding : Inheritance
    {
    public:
        Boarding(Vitesse* vitesse) : State(vitesse, "Boarding") {}
        ~Boarding() {}

        void Initialize() override;
        void Update() override;
        void Finalize() override;
    private:

        bool board_;//乗り込んだかどうか

    };
}

