#include "Engine.h"
#include "imgui/imgui.h"
#include "SceneManager.h"

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
	for (int i = 0; i < children_.size(); i++)
	{
		children_[i].lock()->Initialize();
	}
}

void Actor::Release()
{
	//親子関係を解除し、子を削除する
	SetParent(nullptr);

	for (int i = 0; i < children_.size(); i++)
	{
		if (const auto& child = children_[i].lock())
		{
			Destroy(child);
		}
	}
	children_.clear();

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

	//子のImGui表示
	if (children_.size() > 0)
	{
		ImGui::Text("-------Children-------");
		for (const auto& a : children_)
		{
			const auto& actor = a.lock();
			if (ImGui::TreeNode(actor->name_.c_str()))
			{
				//ImGui::DockSpace(myDockspace, wSize);
				//ImGui::SetNextWindowDockID(myDockspace, ImGuiCond_Once);
				actor->DrawImGui();

				ImGui::TreePop();
			}
		}
	}
}

void Actor::Destroy(std::shared_ptr<Actor> actor)
{
	Engine::sceneManager_->GetActiveScene().DestroyActor(actor);
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
				RemoveParent();
			}

			//親を設定
			parent_ = parent;
			parent->children_.emplace_back(shared_from_this());
		}
	}
}

void Actor::RemoveParent()
{
	//自分と親の関係をどちらからも解除
	if (const auto& p = parent_.lock())
	{
		//親の子一覧から自身を探し、解除
		auto it = p->children_.begin();
		while (it != p->children_.end())
		{
			if ((*it).lock() == shared_from_this())
			{
				p->children_.erase(it);
				break;
			}

			++it;
		}

		parent_.reset();
	}
}

bool Actor::GetActiveInHierarchy() const
{
	if (active_)
	{
		//親のオブジェクトがアクティブかどうかも考慮する
		if (const auto& actor = parent_.lock())
		{
			return actor->GetActiveInHierarchy();
		}
	}
	return active_;
}

