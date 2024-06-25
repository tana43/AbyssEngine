#include "SceneManager.h"
#include "Engine.h"
#include "Scene.h"

#include "TestScene.h"
#include "SceneGame.h"
#include "SceneTitle.h"

#include "RenderManager.h"

#include "imgui/imgui.h"

using namespace std;
using namespace AbyssEngine;

SceneManager::SceneManager()
{
    AddScene(new TestScene,"Test");
    AddScene(new SceneGame,"Game");
    AddScene(new SceneTitle,"Title");
    SetNextScene("Test");
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

        if(activeScene_)activeScene_->Update();
    }
}

void SceneManager::AddScene(Scene* scene, string name)
{
    sceneMap_[name].reset(scene);
    scene->name_ = name;
}

void SceneManager::SetNextScene(std::string name)
{
    nextSceneName_ = name;
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
    if (nextSceneName_.empty())return;

    if (activeScene_)activeScene_->Finalize();

    activeScene_ = sceneMap_[nextSceneName_].get();

    activeScene_->Initialize();

    nextSceneName_ = "";
}
