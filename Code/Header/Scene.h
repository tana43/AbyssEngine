#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include "Actor.h"
 
namespace AbyssEngine
{
    //シーン
    class Scene
    {
    public:
        Scene() {}
        ~Scene() = default;

        std::weak_ptr<Actor> Find(const std::string& name_);  //指定した名前のアクターを返す

        std::string name_;   //シーン名

        std::shared_ptr<Actor> InstanceActor(const std::string& name);      //シーン内にオブジェクト配置する

        void RegistDestroyActor(const std::shared_ptr<Actor>& actor);       //削除するアクターとして登録する基本これを使う
        void DestroyActor(const std::shared_ptr<Actor>& actor);             //アクターを削除する
        void DestroyComponent(const std::shared_ptr<Component>& component); //コンポーネントを削除する
        void DrawWorldOutLinerImGui(); //ImGuiデバッグ表示

        std::string GenerateUniqueName(const std::string& baseName);

    protected:
        virtual void Initialize();  //初期化
        virtual void Update();      //更新
        virtual void Reset();       //シーンをリセット
        virtual void Finalize();    //後始末
        void UpdateRemoveActors();  //アクターを破棄する
        void TimeUpdate();

        virtual void DrawDebug(); //デバッグ表示
        virtual void DrawImGui(); //ImGuiデバッグ表示
        virtual void DrawImGuiAlways();//常に表示させるIMguiデバッグ表示
        void ImGuiSaveAllActors();//すべてのアクターのトランスフォーム情報を保存

    private:


        std::vector<std::shared_ptr<Actor>> actorList_;//シーン内の全アクター

        std::vector<std::shared_ptr<Actor>> destroyActorList_;//破棄するアクター

        std::unordered_map<std::string, int> nameCount_;

        friend class SceneManager;

    private:
        bool imguiFrag_ = false;
    };

}

