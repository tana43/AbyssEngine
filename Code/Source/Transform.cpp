#include "Transform.h"
#include "Component.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;

Matrix Transform::CalcWorldMatrix()
{
    const auto S = Matrix::CreateScale(scale_);
    const auto R = Matrix::CreateFromYawPitchRoll(rotation_.x, rotation_.y, rotation_.z);
    const auto T = Matrix::CreateTranslation(position_);

    //各方向ベクトルの算出
    forward_ = Vector3::Transform(Vector3::Forward, R);
    forward_.Normalize();

    right_ = Vector3::Transform(Vector3::Right, R);
    right_.Normalize();

    up_ = Vector3::Transform(Vector3::Up, R);
    up_.Normalize();

    return S * R * T;
}

Vector3 Transform::GetEulerAngles() const 
{
    return rotation_.To_Euler();
}

bool Transform::DrawImGui()
{
    if (ImGui::TreeNode("Transform"))
    {
        ImGui::DragFloat3("Position", &position_.x, 0.1f, -FLT_MAX, FLT_MAX);
        ImGui::DragFloat3("Scale", &scale_.x, 0.01f, -FLT_MAX, FLT_MAX);
        ImGui::DragFloat3("Rotation", &rotation_.x, 0.01f, -FLT_MAX, FLT_MAX);
        ImGui::DragFloat("ScaleFactor", &scaleFactor_, 0.01f, 0.01f, 100.0f);

        ImGui::TreePop();
    }

    return false;
}
