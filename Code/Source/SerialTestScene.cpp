#include "SerialTestScene.h"
#include "Camera.h"
#include "StaticMesh.h"

using namespace AbyssEngine;

void SerialTestScene::Initialize()
{
    Scene::Initialize();

    //�J����
    const auto& camera_ = InstanceActor("Debug_Camera");
    camera_->AddComponent<Camera>();

    //�e�X�g���f��
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
