#include "SceneGame.h"
#include "Camera.h"

using namespace AbyssEngine;

void SceneGame::Initialize()
{
    Scene::Initialize();

    //ƒJƒƒ‰
    const auto& camera_ = InstanceActor("Debug_Camera");
    camera_->AddComponent<Camera>();
}

void SceneGame::Update()
{

}

void SceneGame::DrawImGui()
{
}