#include "FacilityScene.h"
#include "StaticMesh.h"
#include "Camera.h"
#include "PlayerSoldier.h"
#include "SkeletalMesh.h"
#include "Stage.h"
#include "StageManager.h"   
#include "BotEnemy.h"
#include "RenderManager.h"
#include "Engine.h"
#include "BillboardRenderer.h"
#include "GameUIAdmin.h"
#include "Input.h"
#include "SceneManager.h"

using namespace AbyssEngine;

//std::weak_ptr<Stage> stageCom_F;

void FacilityScene::Initialize()
{
    Scene::Initialize();

    //ポストエフェクト設定
    Engine::renderManager_->GetBufferScene().data_.exposure_ = 3.58f;
    Engine::renderManager_->GetBufferEffects().data_.shadowFilterRadius_ = 4.614f;
    Engine::renderManager_->GetBufferEffects().data_.shadowColor_ = 0.6f;
    Engine::renderManager_->SetCriticalDepthValue(300.0f);

    //カメラ
    const auto& camera = InstanceActor("Debug_Camera_01");
    camera->AddComponent<Camera>();

    //ステージ
    const auto& stageActor = InstanceActor("Facility_Stage");
    //const auto& stageCom = stageActor->AddComponent<Stage>();
    const auto& stageCom = stageActor->AddComponent<Stage>();
    Engine::stageManager_->SetStage(stageCom);

    const auto& faci = stageCom->AddStageModel("Floor_01", "./Assets/Models/Stage/Facility/Facility.gltf");

    //IBL強度設定
    faci->GetComponent<StaticMesh>()->SetIBLIntensity(0.1f);
    faci->GetComponent<StaticMesh>()->SetEmissiveIntensity(120.0f);

    //判定ポリゴンを更新
    stageCom->RegisterTriangles();

    //enemy
    const auto& enemy = InstanceActor("Enemy_Facility_01");
    enemy->AddComponent<BotEnemy>();

    //Player
    const auto& player = InstanceActor("Player");
    const auto& pc = player->AddComponent<Soldier>();

    //板ポリゴン
    /*const auto& plane = InstanceActor("Plane");
    plane->AddComponent<BillboardRenderer>("./Assets/Effects/Texture/Explosion_02.png");*/

    //UI設定
    const auto& ui = InstanceActor("GameUI");
    const auto& uiCom = ui->AddComponent<GameUIAdmin>();
    uiCom->SetPlayer(pc);

    //ロードするシーンを設定
    Engine::sceneManager_->SetLoadScene("Test");
}

void FacilityScene::Update()
{
    if (Keyboard::GetKeyDown(DirectX::Keyboard::Enter))
    {
        //if()
        Engine::sceneManager_->SetNextScene("Test");
    }

}

void FacilityScene::DrawImGui()
{
}

