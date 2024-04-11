#include "TestScene.h"
#include "SpriteRenderer.h"
#include "Camera.h"
#include "SkeltalMesh.h"

using namespace AbyssEngine;

void TestScene::Initialize()
{
    //�J����
    const auto& camera = InstanceActor("MainCamera");
    camera->AddComponent<Camera>();
    camera->GetTransform()->SetPosition(Vector3(0, 3, -10));

    //�e�X�g�p�̃I�u�W�F�N�g
    const auto& p = InstanceActor("testObj");
    p->AddComponent<SkeltalMesh>("./Assets/Moels/latha.fbx");
}

void TestScene::Update()
{

}
