#include "Transform.h"
#include "Component.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;

void Transform::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = std::static_pointer_cast<Transform>(shared_from_this());
}

Matrix Transform::CalcWorldMatrix()
{
    Matrix S, R, T;

    //親子関係の有無
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
        //オイラー角をクォータニオンに変換
        Vector3 euler = { rotation_.x,rotation_.y,rotation_.z };
        const auto scale = scale_ * scaleFactor_;
        S = Matrix::CreateScale(scale);
        R = Matrix::CreateFromQuaternion(Quaternion::Euler(euler));
        T = Matrix::CreateTranslation(position_);
        worldMatrix_ = S * R * T;
    }
    
    //各方向ベクトルの算出
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
    //オイラー角をクォータニオンに変換
    Vector3 euler = { localRotation_.x,localRotation_.y,localRotation_.z };
    Matrix localS = Matrix::CreateScale(localScale_ * localScaleFactor_);
    Matrix localR = Matrix::CreateFromQuaternion(Quaternion::Euler(euler));
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
        ImGui::DragFloat3("Rotation", &rotation_.x, 0.5f, -FLT_MAX, FLT_MAX);
        ImGui::DragFloat("ScaleFactor", &scaleFactor_, 0.01f, 0.01f, 100.0f);

        //親がいるならローカル座標も表示
        if (actor_->GetParent().lock())
        {
            ImGui::Text("-----Local------");
            ImGui::DragFloat3("LPosition", &localPosition_.x, 0.1f, -FLT_MAX, FLT_MAX);
            ImGui::DragFloat3("LScale", &localScale_.x, 0.01f, -FLT_MAX, FLT_MAX);
            ImGui::DragFloat3("LRotation", &localRotation_.x, 0.5f, -FLT_MAX, FLT_MAX);
            ImGui::DragFloat("LScaleFactor", &localScaleFactor_, 0.01f, 0.01f, 100.0f);
        }

        ImGui::TreePop();
    }

    return false;
}
