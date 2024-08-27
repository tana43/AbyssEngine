#pragma once
#include "State.h"
#include "Camera.h"

class Soldier;

//�p����
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

        //ADS���̕��s���x
        const float Max_Move_Speed = 0.5f;

        //�G�C���X�e�[�g���ʂ̃X�e�[�g�ɐ؂�ւ��ŏ�����
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
        //���n���Ă���̍d�����Ԍv��
        float landTimer_;
    };

    class Dodge : Inheritance
    {
    public:
        Dodge(Soldier* soldier) : State(soldier, "Dodge") {}
        ~Dodge() {}

        void Initialize()override;
        void Update()override;
        void Finalize()override;

    private:
    };
};

