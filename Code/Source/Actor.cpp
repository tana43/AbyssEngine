#include "Actor.h"
#include "Engine.h"
#include "Transform.h"

using namespace AbyssEngine;
using namespace std;

void Actor::Initialize()
{
    //各コンポーネントの初期化を呼ぶ
	for (const auto& c : componentList_)
	{
		c->Initialize(dynamic_pointer_cast<Actor>(shared_from_this()));
	}
	//子に対しても行う
	/*for (int i = 0; i < transform_->GetChildCount(); i++)
	{
		transform_->GetChild(i).lock()->actor_->Initialize();
	}*/
}

void AbyssEngine::Actor::Release()
{
	//親子関係を解除し、子を削除する
	/*transform_->SetParent(nullptr);
	while (transform_->GetChildCount())
	{
		for (int i = 0; i < transform_->GetChildCount(); i++)
		{
			if (const auto& child = transform_->GetChild(i).lock())
			{
				Destroy(child->actor);
			}
		}
	}*/

	//各コンポーネントを削除する	
	for (auto& c : componentList_)
	{
		c->actor_.reset(); 
		c->transform_.reset();
		c.reset();
	}
	transform_.reset();
	componentList_.clear();
}

bool Actor::GetActiveInHierarchy() const
{
	if (active_)
	{
		//TODO : 後でやる　親のオブジェクトがアクティブかどうかも考慮する
	}
	return active_;
}