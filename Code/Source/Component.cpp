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
	if (std::dynamic_pointer_cast<Component>(buff))
	{

	}


}