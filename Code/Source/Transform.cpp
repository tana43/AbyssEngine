#include "Transform.h"
#include "Component.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;

Matrix Transform::CalcWorldMatrix()
{
    Matrix S, R, T;

    //�e�q�֌W�̗L��
    if (const auto& parent = actor_->GetParent().lock())
    {
        const Matrix localMatrix = CalcLocalMatrix();
        const auto scale = scale_ * scaleFactor_;
        worldMatrix_ = localMatrix * parent->GetTransform()->GetWorldMatrix();
        worldMatrix_.Decompose(scale_, rotation_, position_);
        
        R = Matrix::CreateFromQuaternion(rotation_);
    }
    else
    {
        const auto scale = scale_ * scaleFactor_;
        S = Matrix::CreateScale(scale);
        R = Matrix::CreateFromYawPitchRoll(rotation_.y, rotation_.x, rotation_.z);
        T = Matrix::CreateTranslation(position_);
        worldMatrix_ = S * R * T;
    }
    
    //�e�����x�N�g���̎Z�o
    forward_ = Vector3::Transform(Vector3::Forward, R);
    forward_.Normalize();

    right_ = Vector3::Transform(Vector3::Right, R);
    right_.Normalize();

    up_ = Vector3::Transform(Vector3::Up, R);
    up_.Normalize();

    return worldMatrix_;
}

Matrix Transform::CalcLocalMatrix()
{
    Matrix localS = Matrix::CreateScale(localScale_ * localScaleFactor_);
    Matrix localR = Matrix::CreateFromQuaternion(localRotation_);
    Matrix localT = Matrix::CreateTranslation(localPosition_);
    localMatrix_ = localS * localR * localT;
    return localMatrix_;
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
