#include "HumanoidWeapon.h"
#include "Engine.h"
#include "imgui/imgui.h"

using namespace AbyssEngine;

HumanoidWeapon::HumanoidWeapon()
{
    Max_Horizontal_Speed = 40.0f;
    acceleration_ = 30.0f;
    deceleration_ = 10.0f;
}

void HumanoidWeapon::Update()
{
    UpdateInputMove();
    Move();
}

bool HumanoidWeapon::DrawImGui()
{
    ImGui::DragFloat3("Velocity", &velocity_.x);

    float speed = velocity_.Length();
    ImGui::SliderFloat("Speed", &speed, 0.0f, Max_Horizontal_Speed);

    ImGui::DragFloat("Max Speed", &Max_Horizontal_Speed, 0.1f, 0.1f);

    ImGui::SliderFloat("Accel", &acceleration_, 0.0f, 100.0f);
    ImGui::SliderFloat("Decel", &deceleration_, 0.0f, 100.0f);

    ImGui::SliderFloat("Climb", &climbSpeed_, 0.0f, 200.0f);

    ImGui::SliderFloat("Rot Speed", &baseRotSpeed_, 0.0f, 1000.0f);
    ImGui::SliderFloat("Max Rot Speed", &Max_Rot_Speed, 0.0f, 1000.0f);
    ImGui::SliderFloat("Min Rot Speed", &Min_Rot_Speed, 0.0f, 1000.0f);

    return true;
}

void HumanoidWeapon::Move()
{
    Character::Move();
}

void HumanoidWeapon::Climb(float amount)
{
    velocity_.y += amount;
}



