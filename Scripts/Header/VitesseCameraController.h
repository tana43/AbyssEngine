#pragma once
#include "ScriptComponent.h"

namespace AbyssEngine
{
    class Camera;
}

class VitesseCameraController : public AbyssEngine::ScriptComponent
{
public:
    //構図の種類
    enum class Composition
    {
        CaptureLeft,
        CaptureRight,
    };

public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

    //アクターの移動処理等が終わってから更新させるためEndUpdateを使用する
    void UpdateEnd()override;


private:
    std::shared_ptr<AbyssEngine::Camera> camera_;
    std::shared_ptr<AbyssEngine::Transform> targetTransform_;

    AbyssEngine::Vector3 targetOffset_;

    AbyssEngine::Vector3 captureLeftOffset_ = { 12.2f,14.5f,0 };
    AbyssEngine::Vector3 captureRightOffset_ = { -12.2f,14.5f,0 };

    //現在の画面構成
    Composition compositoin_ = Composition::CaptureLeft;

    //遷移時間
    const float transTime_ = 4.5f;
    float transTimer_ = 0.0f;

    //構図の変更があったか
    bool isChangeCompo_ = false;
};

