#include "SceneTitle.h"
#include "Camera.h"

using namespace AbyssEngine;

void SceneTitle::Initialize()
{
    //�J����
    const auto& camera_ = InstanceActor("Debug_Camera");
    camera_->AddComponent<Camera>();
}

void SceneTitle::Update()
{
}

void SceneTitle::DrawImGui()
{
}

void SceneTitle::Finalize()
{
}
