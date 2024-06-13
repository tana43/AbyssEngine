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
    //�J����
    const auto& camera_ = InstanceActor("Debug_Camera");
    camera_->AddComponent<Camera>();
    //camera_->GetTransform()->SetPosition(Vector3(0, 3, -10));

    //�e�X�g�p�̃I�u�W�F�N�g
    /*const auto& p0 = InstanceActor("testModel");
    p0->AddComponent<SkeltalMesh>("./Assets/Models/nico.fbx");*/

    //�e�X�g�p�̃I�u�W�F�N�g
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


    const float scale = 0.3f;
    //���w�r��
    for (int i = 0; i < 10; i++)
    {
        std::string name = "Skyscraper_";
        name += std::to_string(i);
        const auto& skyscraper = InstanceActor(name);
        skyscraper->AddComponent<StaticMesh>("./Assets/Models/Stage/Skyscraper_001.glb");
        //skyscraper->GetTransform()->SetScaleFactor(scale);
    }
    //�r��
    for (int i = 0; i < 10; i++) 
    {
        std::string name = "Building_";
        name += std::to_string(i);
        const auto& building = InstanceActor(name);
        building->AddComponent<StaticMesh>("./Assets/Models/Stage/Building_001.glb");
        //building->GetTransform()->SetScaleFactor(scale);
    }
    //��
    //for (int i = 0; i < 30; i++) 
    //{
    //    std::string name = "Tree_";
    //    name += std::to_string(i);
    //    const auto& tree = InstanceActor(name);
    //    tree->AddComponent<StaticMesh>("./Assets/Models/Stage/Tree_001.glb");
    //    //tree->GetTransform()->SetScaleFactor(scale);
    //}
    //

    //�A�X�t�@���g
    for (int i = 0; i < 9; i++) 
    {
        std::string name = "Asphalt_";
        name += std::to_string(i);
        const auto& asphalt = InstanceActor(name);
        asphalt->AddComponent<StaticMesh>("./Assets/Models/Stage/Asphalt.glb");
        asphalt->GetTransform()->SetScaleFactor(30.0f);
    }
    

#if 1//���B�e�X���f��������
    const auto& vitesse = InstanceActor("Vitesse");
    vitesse->AddComponent<Vitesse>();
#endif // 0//���B�e�X���f��������

    //Player
    const auto& player = InstanceActor("Player");
    player->AddComponent<Player>();

    //F-14
    const auto& fighterJet = InstanceActor("F-14A");
    fighterJet->AddComponent<StaticMesh>("./Assets/Models/F-14A.glb");

    swordEfe_ = std::make_unique<Effect>("./Assets/Effects/MoonLightSword.efk");
}

void TestScene::Update()
{
    //DebugRenderer::Get().DrawSphere(Vector3(0, 0, 0), 10.0f, Vector4(1, 0, 0, 0));
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

void TestScene::Finalize()
{
    Scene::Finalize();

    //�Ȃ�Ŏ����ŉ�����Ȃ��̂��������
    swordEfe_.reset();
}
