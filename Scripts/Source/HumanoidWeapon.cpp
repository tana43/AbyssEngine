#include "HumanoidWeapon.h"

void HumanoidWeapon::Update()
{
    MoveUpdate();
}

void HumanoidWeapon::MoveUpdate()
{
    Character::Move();
}
