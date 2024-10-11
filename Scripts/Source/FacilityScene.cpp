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

    //�|�X�g�G�t�F�N�g�ݒ�
    Engine::renderManager_->GetBufferScene().data_.exposure_ = 3.58f;
    Engine::renderManager_->GetBufferEffects().data_.shadowFilterRadius_ = 4.614f;
    Engine::renderManager_->GetBufferEffects().data_.shadowColor_ = 0.6f;
    Engine::renderManager_->SetCriticalDepthValue(300.0f);

    //�J����
    const auto& camera = InstanceActor("Debug_Camera_01");
    camera->AddComponent<Camera>();

    //�X�e�[�W
    const auto& stageActor = InstanceActor("Facility_Stage");
    //const auto& stageCom = stageActor->AddComponent<Stage>();
    const auto& stageCom = stageActor->AddComponent<Stage>();
    Engine::stageManager_->SetStage(stageCom);

    const auto& faci = stageCom->AddStageModel("Floor_01", "./Assets/Models/Stage/Facility/Facility.gltf");

    //IBL���x�ݒ�
    faci->GetComponent<StaticMesh>()->SetIBLIntensity(0.1f);
    faci->GetComponent<StaticMesh>()->SetEmissiveIntensity(120.0f);

    //����|���S�����X�V
    stageCom->RegisterTriangles();

    //enemy
    const auto& enemy = InstanceActor("Enemy_Facility_01");
    enemy->AddComponent<BotEnemy>();

    //Player
    const auto& player = InstanceActor("Player");
    const auto& pc = player->AddComponent<Soldier>();

    //�|���S��
    /*const auto& plane = InstanceActor("Plane");
    plane->AddComponent<BillboardRenderer>("./Assets/Effects/Texture/Explosion_02.png");*/

    //UI�ݒ�
    const auto& ui = InstanceActor("GameUI");
    const auto& uiCom = ui->AddComponent<GameUIAdmin>();
    uiCom->SetPlayer(pc);

    //���[�h����V�[����ݒ�
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

