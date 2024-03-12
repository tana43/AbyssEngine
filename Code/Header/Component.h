#pragma once
#include "Actor.h"

namespace AbyssEngine
{
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
        //std::shared_ptr<Component> AddComponent(const std::string& className_); 

    protected:
        virtual void SetActive(bool value) {};//アクティブ状態を切り替える
        Component() = default;

    private:
        //同じコンポーネントを複数アタッチ可能か
        virtual bool CanMultiple() { return true; }

        //初期化
		virtual bool Initialize(const std::shared_ptr<Actor>& actor) { return true; }

        //ImGui表示用の共通関数
        virtual bool DrawImGui() { return true; }

        friend class Actor;
    };
}

template<class T>
inline std::shared_ptr<T> AbyssEngine::Component::GetComponent()
{
	for (std::shared_ptr<Component>& com : actor_->componentList_)
	{
		std::shared_ptr<T> buff = std::dynamic_pointer_cast<T>(com);
		if (buff != nullptr)
		{
			return buff;
		}
	}
	return nullptr;
}

template<class T>
inline std::shared_ptr<T> AbyssEngine::Component::AddComponent()
{
	std::shared_ptr<T> buff = make_shared<T>();

	//複数アタッチできるか確認
	if (dynamic_pointer_cast<Component>(buff)->CanMultiple())
	{
		dynamic_pointer_cast<Component>(buff)->Initialize(actor_);
		actor_->componentList_.emplace_back(buff);
		return buff;
	}

	//既にアタッチされているか確認
	bool alreadtAttach_ = false;
	for (std::shared_ptr<Component>& com : actor_->componentList_)
	{
		std::shared_ptr<T> _buff = dynamic_pointer_cast<T>(com);
		if (_buff != nullptr)
		{
			alreadtAttach_ = true;
			break;
		}
	}
	if (!alreadtAttach_)
	{
		dynamic_pointer_cast<Component>(buff)->Initialize(actor_);
		actor_->componentList_.emplace_back(buff);
		return buff;
	}

	return nullptr;
}