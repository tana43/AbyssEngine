#include "SerialTestScene.h"
#include "Camera.h"
#include "StaticMesh.h"

using namespace AbyssEngine;

void SerialTestScene::Initialize()
{
    Scene::Initialize();

    //カメラ
    const auto& camera_ = InstanceActor("Debug_Camera");
    camera_->AddComponent<Camera>();

    //テストモデル
    const auto& staticMesh_ = InstanceActor("Static_Model");
    staticMesh_->AddComponent<StaticMesh>("./Assets/Models/Vitesse/Weapon/Vitesse_GunBlade.gltf");
}

void SerialTestScene::Update()
{
}

void SerialTestScene::DrawImGui()
{
}

void SerialTestScene::Finalize()
{
}
