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
#include "SceneManager.h"
#include "SpaceParticleEffect.h"
#include "GameHost.h"

#include "ComputeParticleEmitter.h"
#include "Bloom.h"


#include "imgui/imgui.h"
//#include "StaticMeshBatching.h"

using namespace AbyssEngine;

//std::shared_ptr<Stage> stageCom;
Effekseer::Handle effectHandle = -1;

void TestScene::Initialize()
{
    Scene::Initialize();

    //ポストエフェクト設定
    Engine::renderManager_->GetBufferScene().data_.lightDirection_ = { 0.23f,-0.87f,1.43f };
    Engine::renderManager_->GetBufferScene().data_.exposure_ = 1.0f;
    Engine::renderManager_->GetBufferEffects().data_.shadowFilterRadius_ = 4.0f;
    Engine::renderManager_->GetBufferEffects().data_.shadowColor_ = 0.25f;
    Engine::renderManager_->GetBufferEffects().data_.radialBlurSampleCount_ = 1;
    Engine::renderManager_->GetBufferEffects().data_.radialBlurStrength_ = 0.0f;
    Engine::renderManager_->GetBloom()->bloomIntensity_ = 0.29f;
    Engine::renderManager_->GetBloom()->bloomExtractionThreshold_ = 0.56f;
    Engine::renderManager_->SetCriticalDepthValue(10000.0f);

    //カメラ
    const auto& camera_ = InstanceActor("Debug_Camera");
    const auto& cameraCom = camera_->AddComponent<Camera>();
    Camera::ChangeMainCamera(cameraCom.get());
    //camera_->GetTransform()->SetPosition(Vector3(0, 3, -10));

    //ステージ
    {
        //当たり判定有
        const auto& stageActor = InstanceActor("Test_Stage");
        //const auto& stageCom = stageActor->AddComponent<Stage>();
        const auto& stageCom = stageActor->AddComponent<Stage>();

        const auto& lunar = stageCom->AddStageModel(name_, "./Assets/Models/Stage/Lunar/LunarSurface.gltf");
        lunar->GetTransform()->SetLocalScaleFactor(4.45f);
        lunar->GetComponent<StaticMesh>()->SetIBLIntensity(0.05f);
        Engine::stageManager_->SetStage(stageCom);

        //判定無し
        const auto& station = InstanceActor("Space_Station");
        const auto& m = station->AddComponent<StaticMesh>("./Assets/Models/Stage/SpaceStation/Space_Station_Modules.gltf");
        m->SetEmissiveIntensity(83.0f);

        for (size_t i = 0; i < 5; i++)
        {
            const auto& debris = InstanceActor("Debris");
            const auto& m2 = debris->AddComponent<StaticMesh>("./Assets/Models/Stage/Other/Debris/Debris.gltf");
        }

        const auto& tower = InstanceActor("Tower");
        const auto& m3 = tower->AddComponent<StaticMesh>("./Assets/Models/Stage/Other/Tower/Terraforming_Tower.gltf");

        //山
        {
            const auto& parent = InstanceActor("Mountains");
            for (int i = 0; i < 8; i++)
            {
                const auto& mou = stageCom->AddStageModel("Mountain", "./Assets/Models/Stage/Other/Mountain/Mountain_01.gltf");
                mou->SetParent(parent);
                mou->GetComponent<StaticMesh>()->GetModel()->primitiveConstants_->data_.maxAmbient_ = 0.95f;
                //const auto& mou = InstanceActor("Mountain");
                //const auto& m4 = mou->AddComponent<StaticMesh>("./Assets/Models/Stage/Other/Mountain/Mountain_01.gltf");
            }
        }

        //当たり判定更新
        stageCom->RegisterTriangles();
    }
    

#if 1//ヴィテスモデル仮生成
    const auto& vitesse = InstanceActor("Vitesse");
    const auto& vc = vitesse->AddComponent<Vitesse>();
#endif // 0//ヴィテスモデル仮生成

    //Player
    const auto& player = InstanceActor("Player");
    const auto& pc = player->AddComponent<Soldier>();
    //プレイヤーにヴィテスを設定
    pc->SetMyVitesse(vc);
    //ヴィテスにプレイヤーを設定
    vc->SetPilot(pc);

    //F-14
    //const auto& fighterJet = InstanceActor("F-14A");
    //fighterJet->AddComponent<StaticMesh>("./Assets/Models/F-14A.glb");

    //ボス配置
    const auto& boss = InstanceActor("Boss_Mech_Test");
    const auto& bc = boss->AddComponent<BossMech>();

    //swordEfe_ = std::make_unique<Effect>("./Assets/Effects/Thruster_01.efk");


    //UI設定
    const auto& ui = InstanceActor("GameUI");
    const auto& uiCom = ui->AddComponent<GameUIAdmin>();
    uiCom->SetPlayer(pc);
    uiCom->SetBoss(bc);

    //const auto& staticPlayer = InstanceActor("Player_Static");
    //staticPlayer->AddComponent<StaticMesh>("./Assets/Models/Soldier/Sci_Fi_Soldier_03_Idle.glb");

    //コントローラー振動
    //Input::GetGamePad().SetVibration(0.5f,0.5f);

#if _DEBUG
    const auto& effectEmitter = InstanceActor("Effect");
    //effectEmitter->AddComponent<ParticleEmitter>();
    effectEmitter->AddComponent<ComputeParticleEmitter>();
#endif // DEBUG

    
    //空間パーティクル作成
    const auto& spaceParticleEmitter = InstanceActor("SpaceParticleEmitter");
    spaceParticleEmitter->AddComponent<SpaceParticleEffect>();

    //ゲームクリアなどを管理する進行役を作成
    const auto& host = InstanceActor("GameHost");
    const auto& gh = host->AddComponent<GameHost>();
    gh->SetBoss(bc);

    //画面をフェードインさせる用のスプライト
    const auto& fadeOut = InstanceActor("ScreenFadeOutSprite");
    fadeOutSprite_ = fadeOut->AddComponent<SpriteRenderer>("./Assets/Images/NowLoading.png");

    //BGM再生
    const auto& speaker = InstanceActor("BGM_Player");
    const auto& audio = speaker->AddComponent<AudioSource>();
    audio->SetAssetAudioIndex(AudioIndex::Game);
    audio->SetIsLoop(true);//ループオン
    audio->SetActiveDistanceAttenuation(false);//距離減衰オフ
    audio->SetIsDestroyedAudioStop(true);
    audio->Play();
}

void TestScene::Update()
{
    /*Vector3 hit;
    Vector3 hitn;
    stageCom->RayCast(Vector3(0, 10, 0), Vector3(0, -10, 0), hit, hitn);*/

    //画面をフェードインさせるためにスプライトをフェードアウト
    if (const auto& p = fadeOutSprite_.lock())
    {
        p->FadeOut(0.0f, 0.4f);
    }

    //仮でシーン遷移
    if (Keyboard::GetKeyDown(DirectX::Keyboard::F1))
    {
        Engine::sceneManager_->SetNextScene("Facility");
    }
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
