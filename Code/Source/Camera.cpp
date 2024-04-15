#include "Camera.h"
#include "DXSystem.h"
#include "Actor.h"
#include "imgui/imgui.h"
#include "RenderManager.h"
#include "Engine.h"

using namespace AbyssEngine;
using namespace DirectX;

void Camera::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();

    Engine::renderManager_->Add(static_pointer_cast<Camera>(shared_from_this()));
}

bool Camera::DrawImGui()
{
    if (ImGui::TreeNode("Camera"))
    {
        ImGui::SliderAngle("Fov", &fov_, 30.0f, 120.0f);
        ImGui::DragFloat("Near Z", &nearZ_, 0.01f, 0.01f,1.0f);
        ImGui::DragFloat("Far Z", &farZ_, 1.0f, 0.1f);

        ImGui::TreePop();
    }

    return true;
}

void Camera::Update()
{
    const float aspect = static_cast<float>(DXSystem::GetScreenWidth()) 
        / static_cast<float>(DXSystem::GetScreenHeight()); //‰æ–Ê”ä—¦
    projectionMatrix_ = XMMatrixPerspectiveFovLH(fov_, aspect, nearZ_, farZ_);

    //ƒrƒ…[s—ñì¬
    const Vector3 eye = transform_->GetPosition();
    const Vector3 focus = eye + transform_->GetForward();
    const Vector3 up = transform_->GetUp();

    viewMatrix_ = XMMatrixLookAtLH(eye, focus, up);
    viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}
