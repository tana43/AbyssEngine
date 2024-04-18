#include "TestScene.h"
#include "SpriteRenderer.h"
#include "Camera.h"
#include "SkeltalMesh.h"
#include "FbxMeshData.h"
#include "GltfModel.h"

using namespace AbyssEngine;

void TestScene::Initialize()
{
    //カメラ
    const auto& camera = InstanceActor("MainCamera");
    camera->AddComponent<Camera>();
    camera->GetTransform()->SetPosition(Vector3(0, 3, -10));

    //テスト用のオブジェクト
    /*const auto& p0 = InstanceActor("testModel");
    p0->AddComponent<SkeltalMesh>("./Assets/Models/nico.fbx");*/

    //テスト用のオブジェクト
    const auto& p1 = InstanceActor("testSprite");
    p1->AddComponent<GltfSkeltalMesh>("./Assets/Models/UE/SK_Mannequin.gltf");
}

void TestScene::Update()
{

}
