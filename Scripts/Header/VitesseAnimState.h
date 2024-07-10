#pragma once
#include "State.h"
#include "Animator.h"

//継承元
#define Inheritance  public State<AbyssEngine::Animator>

//アニメーションステートマシーン
//アニメーターの各パラメーターへの受け渡し、
namespace VitesseAnimState
{
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

