#include "Component.h"

using namespace AbyssEngine;
using namespace std;

template<class T>
shared_ptr<T> Component::GetComponent()
{
	for (shared_ptr<Component>& com : actor_->componentList_)
	{
		shared_ptr<T> buff = std::dynamic_pointer_cast<T>(com);
		if (buff != nullptr)
		{
			return buff;
		}
	}
	return nullptr;
}

template<class T>
shared_ptr<T> Component::AddComponent()
{
	shared_ptr<T> buff = make_shared<T>();

	//複数アタッチできるか確認
	if (dynamic_pointer_cast<Component>(buff)->CanMultiple())
	{
		dynamic_pointer_cast<Component>(buff)->Initialize(actor_);
		actor_->componentList_.emplace_back(buff);
		return buff;
	}

	//既にアタッチされているか確認
	bool alreadtAttach_ = false;
	for (shared_ptr<Component>& com : actor_->componentList_)
	{
		shared_ptr<T> _buff = dynamic_pointer_cast<T>(com); 
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