#include "Scene.h"
#include "Component.h"
#include "imgui/imgui.h"

#include "Engine.h"
#include "ScriptComponentManager.h"

using namespace AbyssEngine;
using namespace std;

weak_ptr<Actor> Scene::Find(const std::string& name_)
{
    for (auto& a : actorList_)
    {
        if (a->name_ == name_)
        {
            return a;
        }
    }
    weak_ptr<Actor> a;
    return a;
}

shared_ptr<Actor> Scene::InstanceActor(const std::string& name)
{
    //オブジェクトを生成して登録する
    auto actor = make_shared<Actor>();

    //名前被りが存在しないか
    actor->name_ = GenerateUniqueName(name);
    
    //Jsonファイル初期設定
    actor->jsonFilename_ = "./Resources/Json/";
    actor->jsonFilename_ += actor->name_.c_str();
    string Extension = ".json";
    actor->jsonFilename_ += Extension;

    actor->transform_ = actor->AddComponent<Transform>();
    actorList_.emplace_back(actor);

    return actor;
}

void Scene::DestroyActor(const std::shared_ptr<Actor>& actor)
{
    //解放処理をしてからリストから削除する
    actor->Release();
    const auto itrEnd = actorList_.end();
    for (auto itr = actorList_.begin();itr != itrEnd; ++itr)
    {
        if ((*itr) == actor)
        {
            actorList_.erase(itr);
            return;
        }
    }
}

void Scene::DestroyComponent(const std::shared_ptr<Component>& component)
{
    //コンポーネントを探し出して削除する
    const auto itrEnd = actorList_.end();
    for (auto itr = actorList_.begin(); itr != itrEnd; ++itr)
    {
        if ((*itr) == component->GetActor())
        {
            const auto itrCompEnd = (*itr)->GetComponentList().end();
            for (auto itrComp = (*itr)->GetComponentList().begin();itrComp != itrCompEnd;++itr)
            {
                if (typeid(*(*itrComp)) == typeid(*component))
                {
                    (*itrComp)->GetActor().reset();
                    (*itrComp)->GetTransform().reset();
                    (*itr)->GetComponentList().erase(itrComp);
                    return;
                }
            }
            return;
        }
    }
}

void Scene::Initialize()
{
    for (const auto& a : actorList_)
    {
        if (a->GetParent().expired()) 
        {
            a->Initialize();
        }
    }
}

void Scene::Update()
{
}

void Scene::Reset()
{
    //親子関係を持ったアクターは親から消されるので、親がいていないアクターのみ解放
    vector<shared_ptr<Actor>> noParentList;
    for (const auto& a : actorList_)
    {
        if (a->GetParent().expired())
        {
            noParentList.emplace_back(a);
        }
    }
    for (const auto& a : noParentList)
    {
        a->Release();
    }
    noParentList.clear();

    actorList_.clear();
}

void Scene::Finalize()
{
    /*for (const auto& a : actorList_)
    {
        a->Release();
    }*/
    Reset();

    Engine::scriptComManager_->Clear();
}

void Scene::DrawDebug()
{
    for (const auto& a : actorList_)
    {
        a->DrawDebug();
    }
}

void Scene::DrawImGui()
{

}

void Scene::ImGuiSaveAllActors()
{
    if (ImGui::ButtonDoubleChecking("Save All Actors",imguiFrag_))
    {
        for (const auto& a : actorList_)
        {
            a->transform_->SaveToJson();
        }
    }
}

void Scene::DrawWorldOutLinerImGui()
{
    //ImGuiID myDockspace = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    ImVec2 wSize = { 400.0f,1080.0f };
    for (const auto& a : actorList_)
    {
        //子アクターは親から呼び出せるようにしておく
        if (a->GetParent().lock())continue;

        if (ImGui::BeginMenu(a->name_.c_str()))
        {
            //ImGui::DockSpace(myDockspace, wSize);
            //ImGui::SetNextWindowDockID(myDockspace, ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(1920.0f - wSize.x, 24.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(wSize, ImGuiCond_FirstUseEver);
            ImGui::Begin(a->name_.c_str());
            a->DrawImGui();
            ImGui::End();

            ImGui::EndMenu();
        }
    }
}

std::string Scene::GenerateUniqueName(const std::string& baseName) {
    if (nameCount_[baseName] == 0) {
        nameCount_[baseName] = 1;
        return baseName;
    }
    else {
        int count = ++nameCount_[baseName];
        return baseName + "(" + std::to_string(count) + ")";
    }
}
