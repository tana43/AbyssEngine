#pragma once
#include <memory>
#include <string>

namespace AbyssEngine
{
    class Actor;
    class Transform;

    class Component
    {
    public:
        std::shared_ptr<Actor> actor_; //アタッチしているActor
        std::shared_ptr<Transform> transform_;//アタッチしているActorのTransform

        //Actorにアタッチされたコンポーネントを取得（存在しない場合nullptr）
        template<class T>
        std::shared_ptr<T> GetComponent(); 

        //Actorにコンポーネントをアタッチする（テンプレート版）
        template<class T>
        std::shared_ptr<T> AddComponent();                                      

        //Actorにコンポーネントをアタッチする（クラス名版）
        std::shared_ptr<Component> AddComponent(const std::string& className_); 

    protected:
        virtual void SetActive(bool value) {};//アクティブ状態を切り替える
        Component() = default;

    private:
        //同じコンポーネントを複数アタッチ可能か
        virtual bool CanMultiple() { return true; }

        //初期化
        virtual bool Initialize(const std::shared_ptr<Actor>& actor) {}

        //ImGui表示用の共通関数
        virtual bool DrawImGui() { return true; }

        friend class Actor;
    };
}