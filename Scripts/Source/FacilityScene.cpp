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
#include "SceneTransitionCollider.h"
#include "Bloom.h"
#include "ComputeParticleEmitter.h"
#include "SpriteRenderer.h"

using namespace AbyssEngine;

//std::weak_ptr<Stage> stageCom_F;

void FacilityScene::Initialize()
{
    Scene::Initialize();

    //ポストエフェクト設定
    Engine::renderManager_->GetBufferScene().data_.lightDirection_ = {-1.0f,-1.0f,-0.3f};
    Engine::renderManager_->GetBufferScene().data_.exposure_ = 3.58f;
    Engine::renderManager_->GetBufferEffects().data_.shadowFilterRadius_ = 0.614f;
    Engine::renderManager_->GetBufferEffects().data_.shadowColor_ = 0.44f;
    Engine::renderManager_->GetBloom()->bloomIntensity_ = 0.26f;
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
    faci->GetComponent<StaticMesh>()->GetModel()->primitiveConstants_->data_.minAmbient_ = 0.92f;
    faci->GetComponent<StaticMesh>()->GetModel()->primitiveConstants_->data_.maxAmbient_ = 0.99f;

    //IBL強度設定
    faci->GetComponent<StaticMesh>()->SetIBLIntensity(1.0f);
    faci->GetComponent<StaticMesh>()->SetEmissiveIntensity(120.0f);

    //判定ポリゴンを更新
    stageCom->RegisterTriangles();

    //enemy
    {
        for (int i = 0; i < 3; i++)
        {
            const auto& enemy = InstanceActor("Enemy_Facility");
            enemy->AddComponent<BotEnemy>();
        }
    }

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

    //ロードするシーンを設定 未完成
    //Engine::sceneManager_->SetLoadScene("Test");

    //当たったらシーン遷移する
    const auto& sceneTransActor = InstanceActor("SceneTrans");
    sceneTransActor->AddComponent<SceneTransitionCollider>();

    //画面をフェードインさせる用のスプライト
    const auto& fadeOut = InstanceActor("ScreenFadeOutSprite");
    fadeOutSprite_ = fadeOut->AddComponent<SpriteRenderer>("./Assets/Images/NowLoading.png");


#if _DEBUG
    const auto& effectEmitter = InstanceActor("Effect");
    //effectEmitter->AddComponent<ParticleEmitter>();
    effectEmitter->AddComponent<ComputeParticleEmitter>();
#endif // DEBUG_
}

void FacilityScene::Update()
{
#if _DEBUG
    if (Keyboard::GetKeyDown(DirectX::Keyboard::Enter))
    {
        //if()
        //Engine::sceneManager_->SetNextScene("Test");
    }
#endif // _DEBUG

    //画面をフェードインさせるためにスプライトをフェードアウト
    if (const auto& p = fadeOutSprite_.lock())
    {
        p->FadeOut(0.0f, 0.4f);
    }

    //仮でシーン遷移
    if (Keyboard::GetKeyDown(DirectX::Keyboard::F1))
    {
        Engine::sceneManager_->SetNextScene("Test");
    }

}

void FacilityScene::DrawImGui()
{
}

