#include "TestScene.h"
#include "SpriteRenderer.h"
#include "Camera.h"
#include "SkeletalMesh.h"
#include "StaticMesh.h"
#include "Player.h"
#include "DebugRenderer.h"
#include "Vitesse.h"

#include "imgui/imgui.h"
//#include "StaticMeshBatching.h"

using namespace AbyssEngine;

void TestScene::Initialize()
{
    //カメラ
    const auto& camera_ = InstanceActor("Debug Camera");
    camera_->AddComponent<Camera>();
    camera_->GetTransform()->SetPosition(Vector3(0, 3, -10));

    //テスト用のオブジェクト
    /*const auto& p0 = InstanceActor("testModel");
    p0->AddComponent<SkeltalMesh>("./Assets/Models/nico.fbx");*/

    //テスト用のオブジェクト
    const auto& stage = InstanceActor("Stage");
    stage->AddComponent<StaticMesh>(
        //"./Assets/Models/UE/LV_Soul_Slum.glb"
        //"./Assets/Models/UE/Prewiev_Sci_fi_Base.glb"
        //"./Assets/Models/UE/Prewiev_Sci_fi_Base_Retopo.glb"
        "./Assets/Models/UE/Stage_Retopo.glb"
        //"./Assets/Models/UE/TestMap.glb"
        //"./Assets/Models/UE/AssetsvilleTown.glb"
    );
    stage->GetTransform()->SetScaleFactor(0.56f);
    //stage->GetTransform()->SetScaleFactor(0.43f);
    //p1->AddComponent<GltfSkeltalMesh>(
    //    "./Assets/Models/UE/Prewiev_Sci_fi_Base.glb"
    //    //"./Assets/Models/UE/AssetsvilleTown.glb"
    //    //"./Assets/Models/UE/SKM_Manny.glb"
    //    //"./Assets/Models/CesiumMan.glb"
    //    //"./Assets/Models/DamagedHelmet.glb"
    //    //"./Assets/Models/UE/Prewiev_Sci_fi_Base.glb"
    //);

#if 0//ヴィテスモデル仮生成
    const auto& vitesse = InstanceActor("Vitesse");
    vitesse->AddComponent<Vitesse>();
#endif // 0//ヴィテスモデル仮生成

    ////Player
    //const auto& player = InstanceActor("Player");
    //player->AddComponent<Player>();
    //player->GetTransform()->SetPositionY(4.2f);

    ////F-14
    //const auto& fighterJet = InstanceActor("F-14A");
    //fighterJet->AddComponent<StaticMesh>("./Assets/Models/F-14A.glb");

    swordEfe_ = std::make_unique<Effect>("./Assets/Effects/MoonLightSword.efk");
}

void TestScene::Update()
{
    DebugRenderer::Get().DrawSphere(Vector3(0, 0, 0), 10.0f, Vector4(1, 0, 0, 0));
}

void TestScene::DrawImGui()
{
    static Vector3 pos = {};
    static float scale = 1.0f;

    if(ImGui::Button("Play Effect"))
    {
        swordEfe_->Play(pos, scale);
    }

    ImGui::DragFloat3("Effect Position", &pos.x);
    ImGui::DragFloat("Effect Scale", &scale,0.1f,0.01f);

}

void AbyssEngine::TestScene::Finalize()
{
    Scene::Finalize();

    //なんで解放ないのか分からん
    swordEfe_.reset();
}
