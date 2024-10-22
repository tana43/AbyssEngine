#include "SceneManager.h"
#include "Engine.h"
#include "Scene.h"

#include "TestScene.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "FacilityScene.h"
#include "TestSceneSecond.h"
#include "EffectEditScene.h"

#include "RenderManager.h"

#include "imgui/imgui.h"

using namespace std;
using namespace AbyssEngine;

SceneManager::SceneManager()
{
    AddScene(new TestScene,"Test");
    AddScene(new SceneGame,"Game");
    AddScene(new SceneTitle,"Title");
    AddScene(new FacilityScene,"Facility");
    AddScene(new TestSceneSecond,"TestSceneSecond");
    AddScene(new EffectEditScene,"Effect");
    //SetNextScene("TestSceneSecond");
    //SetNextScene("Title");
    SetNextScene("Test");
    //SetNextScene("Facility");
    //SetNextScene("Effect");
}

SceneManager::~SceneManager()
{
}

void SceneManager::Exit() const
{
    if (activeScene_)
    {
        activeScene_->Finalize();
        activeScene_->Reset();
    }
}

void SceneManager::Update()
{
    if (run)
    {
        ChangeScene();

        if (activeScene_)
        {
            activeScene_->Update();

            activeScene_->UpdateRemoveActors();
        }
    }
}

void SceneManager::TimeUpdate()
{
    if (run)
    {
        if (activeScene_)
        {
            activeScene_->TimeUpdate();
        }
    }
}

void SceneManager::AddScene(Scene* scene, string name)
{
    sceneMap_[name].reset(scene);
    scene->name_ = name;
}

void SceneManager::SetNextScene(std::string name)
{
    if (loadScene_)
    {
        if (loadScene_->name_ != name)
        {
            _ASSERT_EXPR(false, L"別のシーンがロード済です。正しいシーンを設定しなおしてください。");
        }
    }
    nextSceneName_ = name;
}

void AbyssEngine::SceneManager::SetLoadScene(std::string name)
{
    if (sceneMap_.find(name) == sceneMap_.end())
    {
        _ASSERT_EXPR(false, L"ロードするシーンがございません");
    }

    loadScene_ = sceneMap_[name].get();

    //スレッド開始
    thread_ = make_unique<thread>(LoadingScene,this);
}

void SceneManager::DrawImGui()
{
    if (ImGui::BeginMenu("SceneManager"))
    {
        static char sceneName[128] = "Empty";
        ImGui::InputText("Change Scene Name",sceneName,ARRAYSIZE(sceneName));
        if (ImGui::Button("Change Scene"))
        {
            SetNextScene(sceneName);
        }

        ImGui::EndMenu();
    }

    if (activeScene_)
    {
        activeScene_->ImGuiSaveAllActors();
        activeScene_->DrawImGui();
    }
}

void AbyssEngine::SceneManager::DrawImGuiAlways()
{
    if (activeScene_)
    {
        activeScene_->DrawImGuiAlways();
    }
}

void SceneManager::DrawDebug()
{
    if (activeScene_) activeScene_->DrawDebug();
}

void SceneManager::DrawWorldOutLinerImGui()
{
    if (activeScene_)activeScene_->DrawWorldOutLinerImGui();
}

Scene& SceneManager::GetActiveScene()
{
    return *activeScene_;
}

void SceneManager::ChangeScene()
{
    isChangeSceneFrame_ = false;

    if (nextSceneName_.empty())return;

    if (!wait1frame_)
    {
        wait1frame_ = true;
        return;
    }

    if (activeScene_)activeScene_->Finalize();

    activeScene_ = sceneMap_[nextSceneName_].get();

    //ロードしているなら初期化しない
    if (!loadComplete_)
    {
        activeScene_->Initialize();
    }
    else
    {
        thread_->join();
        loadComplete_ = false;
        thread_.reset();

        loadScene_ = nullptr;
    }

    //経過時間をリセット
    Time::SetDeltaTime(0.0f);

    wait1frame_ = false;

    nextSceneName_ = "";

    isChangeSceneFrame_ = true;

}

void AbyssEngine::SceneManager::LoadingScene(SceneManager* sceneManager)
{
    //COM関連の初期化でスレッド毎に呼ぶ必要がある
    CoInitialize(nullptr);

    //シーンの初期化を行う
    sceneManager->loadScene_->Initialize();

    //スレッドが終わる前にCOM関連の終了化
    CoUninitialize();

    //次のシーンの準備完了
    sceneManager->loadComplete_ = true;
}
