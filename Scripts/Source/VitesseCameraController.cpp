#include "VitesseCameraController.h"
#include "Actor.h"
#include "Camera.h"

using namespace AbyssEngine;

void VitesseCameraController::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    ScriptComponent::Initialize(actor);

    camera_ = actor->GetComponent<Camera>();
    camera_->SetFov(DirectX::XMConvertToRadians(80.0f));
    camera_->SetBaseTargetOffset(Vector3(0.8f, 1.4f, 0));
    camera_->SetTargetOffset(Vector3(0.8f, 1.4f, 0));
    camera_->SetViewTarget(transform_.get());
    camera_->SetEnableDebugController(false);

    //camera_->SetCameraLagSpeed();
}

void VitesseCameraController::EndUpdate()
{

}
