#include "TestScene.h"
#include "SpriteRenderer.h"
#include "Camera.h"
#include "SkeletalMesh.h"
#include "StaticMesh.h"
#include "PlayerSoldier.h"
#include "DebugRenderer.h"
#include "Vitesse.h"
#include "MeshCollider.h"
#include "Stage.h"
#include "StageManager.h"
#include "Engine.h"
#include "RenderManager.h"
#include "EffectManager.h"
#include "GameUIAdmin.h"
#include "BossMech.h"
#include "Input.h"

#include "imgui/imgui.h"
//#include "StaticMeshBatching.h"

using namespace AbyssEngine;

//std::shared_ptr<Stage> stageCom;
Effekseer::Handle effectHandle = -1;

void TestScene::Initialize()
{
    Scene::Initialize();

    //ポストエフェクト設定
    Engine::renderManager_->GetBufferScene().data_.exposure_ = 1.0f;
    Engine::renderManager_->GetBufferEffects().data_.shadowFilterRadius_ = 4.0f;
    Engine::renderManager_->GetBufferEffects().data_.shadowColor_ = 0.25f;
    Engine::renderManager_->SetCriticalDepthValue(2000.0f);

    //カメラ
    const auto& camera_ = InstanceActor("Debug_Camera");
    const auto& cameraCom = camera_->AddComponent<Camera>();
    Camera::ChangeMainCamera(cameraCom.get());
    //camera_->GetTransform()->SetPosition(Vector3(0, 3, -10));

    //テスト用のオブジェクト
    /*const auto& p0 = InstanceActor("testModel");
    p0->AddComponent<SkeltalMesh>("./Assets/Models/nico.fbx");*/

    //テスト用のオブジェクト
    //const auto& stage_00 = InstanceActor("Stage");
    //stage_00->AddComponent<StaticMesh>(
    //    //"./Assets/Models/UE/LV_Soul_Slum.glb"
    //    //"./Assets/Models/UE/Prewiev_Sci_fi_Base.glb"
    //    //"./Assets/Models/UE/Prewiev_Sci_fi_Base_Retopo.glb"
    //    "./Assets/Models/UE/Stage_Retopo.glb"
    //    //"./Assets/Models/UE/TestMap.glb"
    //    //"./Assets/Models/UE/AssetsvilleTown.glb"
    //);
    //const auto& stage_01 = InstanceActor("Stage_00");
    //stage_01->AddComponent<StaticMesh>(
    //    "./Assets/Models/Stage/Skyscraper_001.glb"
    //);

#if 0
    const float scale = 0.2f;
    //高層ビル
    for (int i = 0; i < 10; i++)
    {
        std::string name = "Skyscraper_";
        name += std::to_string(i);
        const auto& skyscraper = InstanceActor(name);
        skyscraper->GetTransform()->SetScaleFactor(scale);
        skyscraper->AddComponent<StaticMesh>("./Assets/Models/Stage/Skyscraper_001.glb");
        skyscraper->AddComponent<MeshCollider>("./Assets/Models/Stage/Skyscraper_001_Collision.glb");
    }
    //ビル
    for (int i = 0; i < 10; i++) 
    {
        std::string name = "Building_";
        name += std::to_string(i);
        const auto& building = InstanceActor(name);
        //building->AddComponent<StaticMesh>("./Assets/Models/Stage/Building_001.glb");
        //building->AddComponent<StaticMesh>("./Assets/Models/Stage/Skyscraper_002.glb");
        building->GetTransform()->SetScaleFactor(scale);
        building->AddComponent<StaticMesh>("./Assets/Models/Stage/Skyscraper_003.glb");
        building->AddComponent<MeshCollider>("./Assets/Models/Stage/Skyscraper_003_Collision.glb");
    }
    //木
    //for (int i = 0; i < 30; i++) 
    //{
    //    std::string name = "Tree_";
    //    name += std::to_string(i);
    //    const auto& tree = InstanceActor(name);
    //    tree->AddComponent<StaticMesh>("./Assets/Models/Stage/Tree_001.glb");
    //    //tree->GetTransform()->SetScaleFactor(scale);
    //}

    //アスファルト
    for (int i = 0; i < 9; i++) 
    {
        std::string name = "Asphalt_";
        name += std::to_string(i);
        const auto& asphalt = InstanceActor(name);
        asphalt->AddComponent<StaticMesh>("./Assets/Models/Stage/Asphalt.glb");
        //asphalt->GetTransform()->SetScaleFactor(30.0f);
    }
#else
    const auto& stageActor = InstanceActor("Test_Stage");
    //const auto& stageCom = stageActor->AddComponent<Stage>();
    const auto& stageCom = stageActor->AddComponent<Stage>();

    /*for (int i = 0; i < 10; i++)
    {
        std::string name_ = "Skyscraper_001_";
        name_ += std::to_string(i);
        stageCom->AddStageModel(name_, "./Assets/Models/Stage/Skyscraper_001.glb");
    }
    for (int i = 0; i < 10; i++)
    {
        std::string name_ = "Skyscraper_002_";
        name_ += std::to_string(i);
        stageCom->AddStageModel(name_, "./Assets/Models/Stage/Skyscraper_002.glb");
    }
    for (int i = 0; i < 10; i++)
    {
        std::string name_ = "Office_001_";
        name_ += std::to_string(i);
        stageCom->AddStageModel(name_, "./Assets/Models/Stage/Office_001.glb");
    }*/
    /*int gridSize = 12;
    for (int x = 0; x < gridSize; x++)
    {
        for (int z = 0; z < gridSize; z++)
        {
            float scale = 30.0f;
            float oneGrid = scale * 2;

            std::string name_ = "Asphalt_";
            name_ += std::to_string(x * gridSize + z);
            const auto& asp = stageCom->AddStageModel(name_, "./Assets/Models/Stage/Asphalt.glb");
            asp->GetTransform()->SetLocalScaleFactor(scale);
            asp->GetTransform()->SetLocalPositionX((-(gridSize / 2) * oneGrid) + x * oneGrid);
            asp->GetTransform()->SetLocalPositionZ((-(gridSize / 2) * oneGrid) + z * oneGrid);
            asp->GetComponent<MeshCollider>()->Transform();
        }
    }*/
    const auto& asp = stageCom->AddStageModel(name_, "./Assets/Models/Stage/Asphalt.glb");
    asp->GetTransform()->SetLocalScaleFactor(5000.0f);
    asp->GetComponent<StaticMesh>()->SetIBLIntensity(0.05f);
    stageCom->RegisterTriangles();
    Engine::stageManager_->SetStage(stageCom);
#endif // 0
    

#if 1//ヴィテスモデル仮生成
    const auto& vitesse = InstanceActor("Vitesse");
    const auto& vc = vitesse->AddComponent<Vitesse>();
#endif // 0//ヴィテスモデル仮生成

    //Player
    const auto& player = InstanceActor("Player");
    const auto& pc = player->AddComponent<Soldier>();
    //プレイヤーにヴィテスを設定
    pc->SetMyVitesse(vc);

    //F-14
    //const auto& fighterJet = InstanceActor("F-14A");
    //fighterJet->AddComponent<StaticMesh>("./Assets/Models/F-14A.glb");

    //ボス配置
    const auto& boss = InstanceActor("Boss_Mech_Test");
    boss->AddComponent<BossMech>();

    swordEfe_ = std::make_unique<Effect>("./Assets/Effects/Thruster_01.efk");


    //UI設定
    const auto& ui = InstanceActor("GameUI");
    const auto& uiCom = ui->AddComponent<GameUIAdmin>();
    uiCom->SetPlayer(pc);

    //const auto& staticPlayer = InstanceActor("Player_Static");
    //staticPlayer->AddComponent<StaticMesh>("./Assets/Models/Soldier/Sci_Fi_Soldier_03_Idle.glb");

    //コントローラー振動
    //Input::GetGamePad().SetVibration(0.5f,0.5f);
}

void TestScene::Update()
{
    /*Vector3 hit;
    Vector3 hitn;
    stageCom->RayCast(Vector3(0, 10, 0), Vector3(0, -10, 0), hit, hitn);*/
}

void TestScene::DrawImGui()
{
#if _DEBUG
    //static Vector3 pos = {};
    //static float scale = 1.0f;
    //static Vector3 rot = {};

    //Engine::renderManager_->debugRenderer_->DrawSphere(pos, 0.1f, Vector4(0, 1, 0, 1));
    //if(ImGui::Button("Play Effect"))
    //{
    //    effectHandle = swordEfe_->Play(pos, scale);
    //}

    const auto& m = EffectManager::Instance().GetEffekseerManager();
    //m->SetLocation(effectHandle, pos.x,pos.y,pos.z);
    //m->SetScale(effectHandle, scale,scale,scale);

   ///* auto q = Quaternion::Euler(rot);

   // const Matrix S = Matrix::CreateScale(scale);
   // const Matrix R = Matrix::CreateFromQuaternion(q);
   // const Matrix T = Matrix::CreateTranslation(pos);
   // Matrix W = S * R * T;
   // Effekseer::Matrix43 em = {
   //     W._11,W._12,W._13,
   //     W._21,W._22,W._23,
   //     W._31,W._32,W._33,
   //     W._41,W._42,W._43
   // };
   // m->SetMatrix(effectHandle,em);

   // ImGui::DragFloat3("Effect Position", &pos.x,0.01f);
   // ImGui::DragFloat3("Effect Rotation", &rot.x,0.01f);
   // ImGui::DragFloat("Effect Scale", &scale,0.1f,0.01f);*/
#endif
}

void TestScene::Finalize()
{
    Scene::Finalize();

    //なんで自動で解放しないのか分からん
    swordEfe_.reset();
}
