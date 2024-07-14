#include "FacilityScene.h"
#include "StaticMesh.h"
#include "Camera.h"
#include "Player.h"

using namespace AbyssEngine;

void FacilityScene::Initialize()
{
    //カメラ
    const auto& camera = InstanceActor("Debug_Camera_01");
    camera->AddComponent<Camera>();

    //ステージ
    const auto& floor_01 = InstanceActor("Floor_01");
    //floor_01->AddComponent<StaticMesh>("./Assets/Models/Stage/Facility/Demonstration_Floor_01.glb");
    //floor_01->AddComponent<StaticMesh>("./Assets/Models/Stage/Facility/Demonstration.glb");
    floor_01->AddComponent<StaticMesh>("./Assets/Models/Stage/Facility/Map_BigStarStation_F1.glb");

    const auto& player = InstanceActor("Player_Facility_01");
    player->AddComponent<Player>();
}

void FacilityScene::Update()
{
}

void FacilityScene::DrawImGui()
{
}

void FacilityScene::Finalize()
{
}
