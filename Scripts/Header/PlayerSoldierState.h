#pragma once
#include "State.h"

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
};

