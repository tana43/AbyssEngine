#pragma once
#include "State.h"
#include "Animator.h"

//�p����
#define Inheritance  public State<AbyssEngine::Animator>

//�A�j���[�V�����X�e�[�g�}�V�[��
//�A�j���[�^�[�̊e�p�����[�^�[�ւ̎󂯓n���A
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

