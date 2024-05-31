#include "TestScene.h"
#include "SpriteRenderer.h"
#include "Camera.h"
#include "SkeletalMesh.h"
#include "StaticMesh.h"
#include "Player.h"
#include "DebugRenderer.h"
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

    const auto& vitesse = InstanceActor("Vitesse");
    vitesse->AddComponent<SkeletalMesh>(
        "./Assets/Models/Vitesse/Vitesse_v01.glb"
        //"./Assets/Models/Vitesse/Animations/G_Idle.glb"
    );
    
#if 0
    const auto& p2 = InstanceActor("testGltf_2");
    /*p2->AddComponent<StaticMesh>(
        "./Assets/Models/robot_oj.glb"
    );*/
    p2->AddComponent<SkeletalMesh>(
        //"./Assets/Models/Nico/nico_full_body.glb"
        "./Assets/Models/UE/Manny/Manny_Idle.glb"
        //"./Assets/Models/.glb"
    );
    SkeletalMesh* model = p2->GetComponent<SkeletalMesh>().get();
    //model->AppendAnimations({
    //    "./Assets/Models/Nico/nico_full_body_Anim_NIC_Attack.glb",
    //    "./Assets/Models/Nico/Hip_Hop_Dancing.glb",
    //    "./Assets/Models/Nico/Death_From_Right.glb" });
    model->AppendAnimations({
        "./Assets/Models/UE/Manny/Manny_Walk.glb",
        "./Assets/Models/UE/Manny/Manny_Run.glb"
        });

    const auto& playerCamera = InstanceActor("Player Camera");
    playerCamera->AddComponent<Camera>();
    playerCamera->SetParent(p2);
#else
    //PlayerComponentによるプレイヤーの実装

    const auto& player = InstanceActor("Player");
    player->AddComponent<Player>();
    player->GetTransform()->SetPositionY(4.2f);
#endif // 0

    
}

void TestScene::Update()
{
    DebugRenderer::Get().DrawSphere(Vector3(0, 0, 0), 10.0f, Vector4(1, 0, 0, 0));
}
