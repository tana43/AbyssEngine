#include "SceneTitle.h"
#include "Camera.h"
#include "SpriteRenderer.h"
#include "Keyboard.h"
#include "SceneManager.h"
#include "Engine.h"
#include "Input.h"
#include "AudioSource.h"
#include "RenderManager.h"
#include "StaticMesh.h"
#include "SkeletalMesh.h"
#include "Bloom.h"

using namespace AbyssEngine;

void SceneTitle::Initialize()
{
    Scene::Initialize();

    //カメラ
    {
        const auto& camera = InstanceActor("Camera_Title");
        const auto& c = camera->AddComponent<Camera>();
        Engine::renderManager_->ChangeMainCamera(c.get());

#if _DEBUG
        c->SetEnableDebugController(true);
#else
        c->SetEnableDebugController(false);
#endif // _DEBUG

    }

    //タイトル画像
    //const auto& titleSprite_ = InstanceActor("Title_Sprite");
    //titleSprite_->AddComponent<SpriteRenderer>("./Assets/Images/Title.png");

    const auto& loadSprite = InstanceActor("Load");
    loadSprite_ = loadSprite->AddComponent<SpriteRenderer>("./Assets/Images/NowLoading.png");
    loadSprite_.lock()->SetColorAlpha(0.0f);
    loadSprite_.lock()->SetEnable(false);

    //スタートボタンを催促するテキスト
    const auto& start = InstanceActor("Start");
    startSprite_ = start->AddComponent<SpriteRenderer>("./Assets/Images/Start.png");

    //BGM再生
    //const auto& speaker = InstanceActor("BGM_Player");
    //const auto& audio = speaker->AddComponent<AudioSource>();
    //audio->SetAssetAudioIndex(AudioIndex::Game);
    //audio->SetIsLoop(true);//ループオン
    //audio->SetActiveDistanceAttenuation(false);//距離減衰オフ
    //audio->Play();


    //背景アクター配置
    {
        const auto& a1 = InstanceActor("Vitesse");
        const auto& m1 = a1->AddComponent<SkeletalMesh>("./Assets/Models/Vitesse/Vitesse_UE_01_Stand.gltf");
        //m1->GetAnimator()->AppendAnimation("./Assets/Models/Vitesse/Vitesse_UE_01_HighSpeedFlight_F.gltf","Dash");

        const auto& a2 = InstanceActor("Stage");
        const auto& m2 = a2->AddComponent<StaticMesh>("./Assets/Models/Stage/Facility/Facility.gltf");

        //const auto& a3 = InstanceActor("Mech");
        //const auto& m3 = a3->AddComponent<SkeletalMesh>("./Assets/Models/Enemy/Boss/Mech_Idle.gltf");
    }

    const auto& speaker = InstanceActor("SE_Player");
    const auto& audio = speaker->AddComponent<AudioSource>();
    audio->SetAssetAudioIndex(AudioIndex::Melee_Hit_01);

    isChangeScene_ = false;
}

void SceneTitle::Update()
{
    Scene::Update();

    if (Input::GameSupport::GetStartButton())
    {
        isChangeScene_ = true;
        //Engine::sceneManager_->SetNextScene("Facility");
        //Engine::sceneManager_->SetNextScene("Test");
        if (const auto& l = loadSprite_.lock())
        {
            l->SetEnable(true);
        }
    }

    if (isChangeScene_)
    {
        if (const auto& s = startSprite_.lock())
        {
            s->FadeOut(0.0f, 3.0f);
        }

        if (const auto& l = loadSprite_.lock())
        {
            if (l->FadeIn(1.0f, 1.0f))
            {
                Engine::sceneManager_->SetNextScene("Facility");
            }
        }
    }

    //ブルームの値をいじってそれっぽく
    //0.4 0.25
    static float timer = 0.0;
    timer += Time::GetDeltaTime();
    Engine::renderManager_->GetBloom()->bloomIntensity_ = 0.25f + 0.15f * fabsf(sinf(timer));
}

void SceneTitle::DrawImGui()
{
    Scene::DrawDebug();
}

void SceneTitle::Finalize()
{
    Scene::Finalize();
}
