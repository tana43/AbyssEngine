#include "FacilityScene.h"
#include "StaticMesh.h"
#include "Camera.h"
#include "PlayerSoldier.h"
#include "SkeletalMesh.h"
#include "Stage.h"
#include "StageManager.h"   

using namespace AbyssEngine;

std::shared_ptr<Stage> stageCom_F;

void FacilityScene::Initialize()
{
    //カメラ
    const auto& camera = InstanceActor("Debug_Camera_01");
    camera->AddComponent<Camera>();

    //ステージ
    const auto& stageActor = InstanceActor("Facility_Stage");
    //const auto& stageCom = stageActor->AddComponent<Stage>();
    stageCom_F = stageActor->AddComponent<Stage>();
    StageManager::Instance().AddStage(stageActor);

    stageCom_F->AddStageModel("Floor_01", "./Assets/Models/Stage/Facility/Facility.glb");
    //const auto& floor_01 = InstanceActor("Floor_01");
    ////floor_01->AddComponent<StaticMesh>("./Assets/Models/Stage/Facility/Demonstration_Floor_01.glb");
    ////floor_01->AddComponent<StaticMesh>("./Assets/Models/Stage/Facility/Demonstration.glb");
    //floor_01->AddComponent<StaticMesh>("./Assets/Models/Stage/Facility/Facility.glb");
    ////floor_01->AddComponent<StaticMesh>("./Assets/Models/Stage/Facility/Map_BigStarStation_F1.glb");
    stageCom_F->RegisterTriangles();


    //const auto& enemy = InstanceActor("Enemy_Facility_01");
    //enemy->AddComponent<SkeletalMesh>("./Assets/Models/Enemy/Bot_2024_07_28_2.glb");

    //Player
    const auto& player = InstanceActor("Player_Facility");
    const auto& pc = player->AddComponent<Soldier>();
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
