#include "TestScene.h"
#include "SpriteRenderer.h"
#include "Camera.h"
#include "SkeltalMesh.h"

using namespace AbyssEngine;

void TestScene::Initialize()
{
    //カメラ
    const auto& camera = InstanceActor("MainCamera");
    camera->AddComponent<Camera>();
    camera->GetTransform()->SetPosition(Vector3(0, 3, -10));

    //テスト用のオブジェクト
    const auto& p = InstanceActor("testObj");
    p->AddComponent<SkeltalMesh>("./Assets/Moels/latha.fbx");
}

void TestScene::Update()
{

}
