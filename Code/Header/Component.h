#pragma once
#include "Object.h"

namespace AbyssEngine
{
    class Transform;
    class Actor;

    class Component : public Object
    {
    public:
        //Actorにアタッチされたコンポーネントを取得（存在しない場合nullptr）
        template<class T>
        std::shared_ptr<T> GetComponent(); 

        //Actorにコンポーネントをアタッチする（テンプレート版）
        template<class T>
        std::shared_ptr<T> AddComponent();                                      

        //Actorにコンポーネントをアタッチする（クラス名版）
        //std::shared_ptr<Component> AddComponent(const std::string& className_); 

    public:
        [[nodiscard]] std::shared_ptr<Actor>& GetActor() { return actor_; }
        [[nodiscard]] std::shared_ptr<Transform>& GetTransform() { return transform_; }

    protected:
        virtual void SetActive(bool value) {};//アクティブ状態を切り替える
        Component() = default;

    private:

        //同じコンポーネントを複数アタッチ可能か
        virtual bool CanMultiple() { return true; }

        //初期化
		virtual void Initialize(const std::shared_ptr<Actor>& actor) {}

        //ImGui表示用の共通関数
        virtual bool DrawImGui() { return true; }

        friend class Actor;

    protected:
        std::shared_ptr<Actor> actor_; //アタッチしているActor
        std::shared_ptr<Transform> transform_;//アタッチしているActorのTransform
    };
}
