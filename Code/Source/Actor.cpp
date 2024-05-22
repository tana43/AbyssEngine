#include "Actor.h"
#include "Engine.h"
#include "imgui/imgui.h"

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

void Actor::Release()
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

void Actor::DrawImGui()
{
	for (auto& c : componentList_)
	{
		ImGui::SetNextItemOpen(true,ImGuiCond_Once);
		c->DrawImGui();
	}
}

std::weak_ptr<Actor> Actor::GetParent() const
{
	return parent_;
}

void Actor::SetParent(const std::shared_ptr<Actor>& parent)
{
	if (parent != nullptr)
	{
		if (parent != parent_.lock())
		{
			//親ポインタの寿命が切れている場合、親子関係を解除する
			if (!parent_.expired())
			{
				
			}
		}
	}
}

bool Actor::GetActiveInHierarchy() const
{
	if (active_)
	{
		//TODO : 後でやる　親のオブジェクトがアクティブかどうかも考慮する
	}
	return active_;
}

