#include "TestScene.h"
#include "SpriteRenderer.h"
#include "Camera.h"
#include "SkeltalMesh.h"
#include "MeshData.h"

using namespace AbyssEngine;

void TestScene::Initialize()
{
    //�J����
    const auto& camera = InstanceActor("MainCamera");
    camera->AddComponent<Camera>();
    camera->GetTransform()->SetPosition(Vector3(0, 3, -10));

    //�e�X�g�p�̃I�u�W�F�N�g
    const auto& p0 = InstanceActor("testModel");
    p0->AddComponent<SkeltalMesh>("./Assets/Models/nico.fbx");

    //�e�X�g�p�̃I�u�W�F�N�g
    /*const auto& p1 = InstanceActor("testSprite");
    p1->AddComponent<SpriteRenderer>("./Assets/Images/Image_00.png");*/
}

void TestScene::Update()
{

}
