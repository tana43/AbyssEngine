#include "Scene.h"
#include "Component.h"

using namespace AbyssEngine;
using namespace std;

weak_ptr<Actor> Scene::Find(const std::string& name)
{
    for (auto& a : actorList_)
    {
        if (a->name_ == name)
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
    actor->AddComponent<Transform>();
    actor->name_ = name;
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
        if ((*itr) == component->actor_)
        {
            const auto itrCompEnd = (*itr)->componentList_.end();
            for (auto itrComp = (*itr)->componentList_.begin();itrComp != itrCompEnd;++itr)
            {
                if (typeid(*(*itrComp)) == typeid(*component))
                {
                    (*itrComp)->actor_.reset();
                    (*itrComp)->transform_.reset();
                    (*itr)->componentList_.erase(itrComp);
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
        //TODO : 後で親を持たないものだけ実行する
        /*if (a->transform_->GetParent().expired()) 
        {
            a->Initialize();
        }*/
        a->Initialize();
    }
}

void Scene::Update()
{
}

void Scene::Reset()
{
#if 0
    vector<shared_ptr<Actor>> noParentList;
    for (const auto& a : actorList_)
    {
        if (a->transform->Get_Parent().expired())
        {
            noParentList.emplace_back(a);
        }
    }
    for (const auto& a : noParentList)
    {
        a->Release();
    }
    noParentList.clear();
#else
    for (const auto& a : actorList_)
    {
        a->Release();
    }
#endif // 0

    actorList_.clear();
}

void Scene::Finalize()
{

}
