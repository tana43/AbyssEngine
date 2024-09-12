#pragma once
#include "State.h"
#include "Animator.h"

//継承元
#define Inheritance  public State<AbyssEngine::Animator>

//アニメーションステートマシーン
//アニメーターの各パラメーターへの受け渡し、
namespace VitesseAnimState
{
    //アニメーションに関する処理が必要ないとき
    class AnimDefault : Inheritance
    {
    public:
        AnimDefault(AbyssEngine::Animator* animator) : State(animator, "Default") {}
        ~AnimDefault() {}

        void Initialize()override {}
        void Update()override {}
        void Finalize()override {}
    };

    class AnimGroundMove : Inheritance
    {
    public:
        AnimGroundMove(AbyssEngine::Animator* animator) : State(animator, "GroundMove") {}
        ~AnimGroundMove() {}

        void Initialize()override;
        void Update()override;
        void Finalize()override;
    };

    class AnimFlyMove : Inheritance
    {
    public:
        AnimFlyMove(AbyssEngine::Animator* animator) : State(animator, "FlyMove") {}
        ~AnimFlyMove() {}

        void Initialize()override;
        void Update()override;
        void Finalize()override;
    };
}

