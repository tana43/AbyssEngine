#pragma once
#include "ScriptComponent.h"

namespace AbyssEngine
{
    class Camera;
}

class VitesseCameraController : public AbyssEngine::ScriptComponent
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

    //アクターの移動処理等が終わってから更新させるためEndUpdateを使用する
    void EndUpdate();


private:
    std::shared_ptr<AbyssEngine::Camera> camera_;
    std::shared_ptr<AbyssEngine::Transform> targetTransform_;
};

