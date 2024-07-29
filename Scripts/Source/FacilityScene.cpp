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
    //�|�X�g�G�t�F�N�g�ݒ�
    Engine::renderManager_->GetBufferScene().data_.exposure_ = 3.58f;
    Engine::renderManager_->GetBufferEffects().data_.shadowFilterRadius_ = 4.614f;
    Engine::renderManager_->SetCriticalDepthValue(300.0f);

    //�J����
    const auto& camera = InstanceActor("Debug_Camera_01");
    camera->AddComponent<Camera>();

    //�X�e�[�W
    const auto& stageActor = InstanceActor("Facility_Stage");
    //const auto& stageCom = stageActor->AddComponent<Stage>();
    stageCom_F = stageActor->AddComponent<Stage>();
    StageManager::Instance().AddStage(stageActor);

    const auto& faci = stageCom_F->AddStageModel("Floor_01", "./Assets/Models/Stage/Facility/Facility.glb");
    //IBL���x�ݒ�
    faci->GetComponent<StaticMesh>()->SetIBLIntensity(0.14f);

    //����|���S�����X�V
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
