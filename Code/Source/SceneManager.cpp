#include "SceneManager.h"
#include "Engine.h"
#include "Scene.h"

#include "TestScene.h"
#include "RenderManager.h"

using namespace std;
using namespace AbyssEngine;

SceneManager::SceneManager()
{
    AddScene(new TestScene,"test");
    SetNextScene("test");
}

SceneManager::~SceneManager()
{
}

void SceneManager::Exit() const
{
    if (activeScene_)activeScene_->Reset();
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
    if (activeScene_)activeScene_->DrawImGui();
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
