#pragma once
#include "State.h"
#include "Camera.h"

class Soldier;

//継承元
#define Inheritance  public State<Soldier>

namespace SoldierState
{
    class Move : Inheritance
    {
    public:
        Move(Soldier* soldier) : State(soldier, "Move") {}
        ~Move() {}

        void Initialize()override;
        void Update()override;
        void Finalize()override;
    };

    class Aim : Inheritance
    {
    public:
        Aim(Soldier* soldier) : State(soldier, "Aim") {}
        ~Aim() {}

        void Initialize()override;
        void Update()override;
        void Finalize()override;

    private:
        const AbyssEngine::Camera::ZoomParam zoomParam =
        {
            0.2f,{0,0,0},{0.25f,0.6f,0.0f},0.2f
        };

        //ADS時の歩行速度
        const float Max_Move_Speed = 0.5f;

        //エイムステートが別のステートに切り替わる最小時間
        const float Minimum_Time = 0.15f;
    };

    class Jump : Inheritance
    {
    public:
        Jump(Soldier* soldier) : State(soldier, "Jump") {}
        ~Jump() {}

        void Initialize()override;
        void Update()override;
        void Finalize()override;

    private:
        //着地してからの硬直時間計測
        float landTimer_;
    };

    class Dodge : Inheritance
    {
    public:
        enum class Direction
        {
            Back,
            Forward,
            Right,
            Left
        };

        Dodge(Soldier* soldier) : State(soldier, "Dodge") {}
        ~Dodge() {}

        void Initialize()override;
        void Update()override;
        void Finalize()override;

        //進行方向から回避先の４方向
        Direction DirectionJudge(const AbyssEngine::Vector3& moveVec);

    private:
        //回避キャンセル可能タイム
        const float Cancel_Time = 0.2f;

        //二回目の回避を使用したか
        bool secondDodge_ = false;
    };
};

