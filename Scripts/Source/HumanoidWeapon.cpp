#include "HumanoidWeapon.h"
#include "Engine.h"

using namespace AbyssEngine;

HumanoidWeapon::HumanoidWeapon()
{
    Max_Horizontal_Speed = 40.0f;
    deceleration_ = 1.5f;
}

void HumanoidWeapon::Update()
{
    UpdateInputMove();
    Move();
}

void HumanoidWeapon::Move()
{
    Character::Move();
}



