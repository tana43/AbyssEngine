#include "SceneTitle.h"
#include "Camera.h"
#include "SpriteRenderer.h"
#include "Keyboard.h"
#include "SceneManager.h"
#include "Engine.h"

using namespace AbyssEngine;

void SceneTitle::Initialize()
{
    Scene::Initialize();

    //カメラ
    const auto& camera_ = InstanceActor("Debug_Camera_Title");
    camera_->AddComponent<Camera>();

    //タイトル画像
    const auto& titleSprite_ = InstanceActor("Title_Sprite");
    titleSprite_->AddComponent<SpriteRenderer>("./Assets/Images/Proto_Load_01.png");
}

void SceneTitle::Update()
{
    Scene::Update();

    if (Keyboard::GetKeyDown(DirectX::Keyboard::Enter))
    {
        Engine::sceneManager_->SetNextScene("Test");
    }
}

void SceneTitle::DrawImGui()
{
    Scene::DrawDebug();
}

void SceneTitle::Finalize()
{
    Scene::Finalize();
}
