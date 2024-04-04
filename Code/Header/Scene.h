#pragma once
#include <string>
#include <memory>
#include "Actor.h"
 
namespace AbyssEngine
{
    //シーン
    class Scene
    {
    public:
        std::weak_ptr<Actor> Find(const std::string& name);  //指定した名前のアクターを返す

        std::string name;   //シーン名

    protected:
        std::shared_ptr<Actor> InstanceActor(const std::string& name);      //シーン内にオブジェクト配置する
        void DestroyActor(const std::shared_ptr<Actor>& actor);             //アクターを削除する
        void DestroyComponent(const std::shared_ptr<Component>& component); //コンポーネントを削除する

        virtual void Initialize();  //初期化
        virtual void Update();      //更新
        virtual void Reset();       //シーンをリセット
        virtual void Finalize();    //後始末

        virtual void DrawImGui(); //ImGuiデバッグ表示

        std::vector<std::shared_ptr<Actor>> actorList_; //シーン内の全アクター

        friend class SceneManager;
    };

}

