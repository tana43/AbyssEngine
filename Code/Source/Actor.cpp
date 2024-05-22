#include "Actor.h"
#include "Engine.h"
#include "imgui/imgui.h"

using namespace AbyssEngine;
using namespace std;

void Actor::Initialize()
{
    //�e�R���|�[�l���g�̏��������Ă�
	for (const auto& c : componentList_)
	{
		c->Initialize(dynamic_pointer_cast<Actor>(shared_from_this()));
	}
	//�q�ɑ΂��Ă��s��
	/*for (int i = 0; i < transform_->GetChildCount(); i++)
	{
		transform_->GetChild(i).lock()->actor_->Initialize();
	}*/
}

void Actor::Release()
{
	//�e�q�֌W���������A�q���폜����
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

	//�e�R���|�[�l���g���폜����	
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
			//�e�|�C���^�̎������؂�Ă���ꍇ�A�e�q�֌W����������
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
		//TODO : ��ł��@�e�̃I�u�W�F�N�g���A�N�e�B�u���ǂ������l������
	}
	return active_;
}

