#include "VitesseCameraController.h"
#include "Actor.h"
#include "Camera.h"

using namespace AbyssEngine;

void VitesseCameraController::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    ScriptComponent::Initialize(actor);

    //ターゲットオフセット設定
    targetOffset_ = captureLeftOffset_;

    //カメラの各項目設定
    camera_ = actor->GetComponent<Camera>();
    camera_->SetFov(DirectX::XMConvertToRadians(80.0f));
    camera_->SetBaseTargetOffset(targetOffset_);
    camera_->SetTargetOffset(targetOffset_);
    camera_->SetEnableDebugController(false);
    camera_->SetCameraLagSpeed(Vector3(2.0f, 2.0f, 1.3f));
    camera_->SetArmLength(18.0f);
    camera_->SetBaseArmLength(18.0f);
    camera_->SetActiveZoomTargetOffset(false);
    targetTransform_ = camera_->GetViewTarget()->GetTransform();
}

void VitesseCameraController::UpdateEnd()
{
    //ビューポート座標からターゲットが画面中央にいているか判定処理
    //現在、カメラのどちら側に寄せるように設定されているかを確認し、
    //左から右へ動けば構図が左右入れ替わるように処理をする

    targetOffset_ = camera_->GetTargetOffset();

    float viewportPosX = camera_->WorldToViewportPosition(targetTransform_->GetPosition()).x;

    switch (compositoin_)
    {
        // ヴィテスを左に捉えた構図
    case VitesseCameraController::Composition::CaptureLeft:

        if (viewportPosX > 0.45f)
        {
            if (!isChangeCompo_)
            {
                compositoin_ = Composition::CaptureRight;
                transTimer_ = 0.0f;
                isChangeCompo_ = true;
                break;
            }
        }
        else
        {
            isChangeCompo_ = false;
        }

        if (transTimer_ > transTime_)
        {
            targetOffset_ = captureLeftOffset_;
        }
        else
        {
            float weight = min(transTimer_ / transTime_,1.0f);
            //weight *= weight;
            targetOffset_ = Vector3::Lerp(targetOffset_, captureLeftOffset_, weight);
        }

        break;

        // ヴィテスを右に捉えた構図
    case VitesseCameraController::Composition::CaptureRight:

        if (viewportPosX < 0.55f)
        {
            if (!isChangeCompo_)
            {
                compositoin_ = Composition::CaptureLeft;
                transTimer_ = 0.0f;
                isChangeCompo_ = true;
                break;
            }
        }
        else
        {
            isChangeCompo_ = false;
        }

        if (transTimer_ > transTime_)
        {
            targetOffset_ = captureRightOffset_;
        }
        else
        {
            float weight = min(transTimer_ / transTime_,1.0f);
            //weight *= weight;
            targetOffset_ = Vector3::Lerp(targetOffset_, captureRightOffset_, weight);
        }

        break;
    }

    camera_->SetTargetOffset(targetOffset_);
    
    transTimer_ += actor_->GetDeltaTime();
}
