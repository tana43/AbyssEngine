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
    camera_->SetCameraLagSpeed(Vector3(3.25f, 2.0f, 2.0f));
    targetTransform_ = camera_->GetViewTarget()->GetTransform();
}

void VitesseCameraController::EndUpdate()
{
    //ビューポート座標からターゲットが画面中央にいているか判定処理
    //現在、カメラのどちら側に寄せるように設定されているかを確認し、
    //左から右へ動けば構図が左右入れ替わるように処理をする


}
