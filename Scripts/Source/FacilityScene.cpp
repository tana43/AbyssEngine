#include "FacilityScene.h"
#include "StaticMesh.h"
#include "Camera.h"
#include "PlayerSoldier.h"
#include "SkeletalMesh.h"
#include "Stage.h"
#include "StageManager.h"   
#include "EnemyBot.h"
#include "RenderManager.h"
#include "Engine.h"

using namespace AbyssEngine;

std::shared_ptr<Stage> stageCom_F;

void FacilityScene::Initialize()
{
    //ポストエフェクト設定
    Engine::renderManager_->GetBufferScene().data_.exposure_ = 3.58f;
    Engine::renderManager_->GetBufferEffects().data_.shadowFilterRadius_ = 4.614f;
    Engine::renderManager_->SetCriticalDepthValue(300.0f);

    //カメラ
    const auto& camera = InstanceActor("Debug_Camera_01");
    camera->AddComponent<Camera>();

    //ステージ
    const auto& stageActor = InstanceActor("Facility_Stage");
    //const auto& stageCom = stageActor->AddComponent<Stage>();
    stageCom_F = stageActor->AddComponent<Stage>();
    StageManager::Instance().AddStage(stageActor);

    const auto& faci = stageCom_F->AddStageModel("Floor_01", "./Assets/Models/Stage/Facility/Facility.glb");
    //IBL強度設定
    faci->GetComponent<StaticMesh>()->SetIBLIntensity(0.14f);

    //判定ポリゴンを更新
    stageCom_F->RegisterTriangles();


    const auto& enemy = InstanceActor("Enemy_Facility_01");
    enemy->AddComponent<EnemyBot>();

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
