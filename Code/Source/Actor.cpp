#include "Engine.h"
#include "imgui/imgui.h"
#include "SceneManager.h"

#include "ScriptComponent.h"

#include <iostream>
#include <fstream>

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
	for (int i = 0; i < children_.size(); i++)
	{
		children_[i].lock()->Initialize();
	}
}

void Actor::Release()
{
	//�e�q�֌W���������A�q���폜����
	SetParent(nullptr);

	for (int i = 0; i < children_.size(); i++)
	{
		if (const auto& child = children_[i].lock())
		{
			Destroy(child);
		}
	}
	children_.clear();

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
		//ImGui�\��
		ImGui::SetNextItemOpen(true,ImGuiCond_Once);
		c->DrawImGui();
	}

	//�q��ImGui�\��
	if (children_.size() > 0)
	{
		ImGui::Text("-------Children-------");
		for (const auto& a : children_)
		{
			const auto& actor = a.lock();
			if (ImGui::BeginMenu(actor->name_.c_str()))
			{
				static ImVec2 wSize = { 400.0f,1080.0f };
				ImGui::SetNextWindowPos(ImVec2(1920.0f - wSize.x * 2 - 20.0f, 24.0f), ImGuiCond_::ImGuiCond_Once);
				ImGui::SetNextWindowSize(wSize, ImGuiCond_FirstUseEver);
				ImGui::Begin(actor->name_.c_str());
				actor->DrawImGui();
				//ImGui::DockSpace(myDockspace, wSize);
				//ImGui::SetNextWindowDockID(myDockspace, ImGuiCond_Once);
				ImGui::End();

				ImGui::EndMenu();
			}
		}
	}
}

void Actor::DrawDebug()
{
	for (auto& c : componentList_)
	{
		//�f�o�b�O�\��
		c->DrawDebug();
	}
}

nlohmann::json Actor::ReadAndCreateJsonFile()
{
	//�t�@�C���̓ǂݍ���
	ifstream ifs(jsonFilename_);
	if (ifs.good())
	{
		//�t�@�C���ǂݍ���
		nlohmann::json mJson;
		ifs >> mJson;
		return mJson;
	}
	else
	{
		//�Ȃ���΍쐬���āA���json��Ԃ�
		ofstream writingFile;
		writingFile.open(jsonFilename_, ios::out);
		writingFile.close();

		return nlohmann::json();
	}
}

nlohmann::json AbyssEngine::Actor::ReadingJsonFile(bool& exist)
{
	//�t�@�C�����݃t���O��True��
	exist = true;

	//�t�@�C���̓ǂݍ���
	ifstream ifs(jsonFilename_);
	if (ifs.good())
	{
		//�t�@�C���ǂݍ���
		nlohmann::json mJson;
		ifs >> mJson;
		return mJson;
	}
	else
	{
		//���݃t���O��false�ɂ��A���json��Ԃ�
		exist = false;
		return nlohmann::json();
	}
}

void Actor::WritingJsonFile(const nlohmann::json& json)
{
	ofstream writingFile;
	writingFile.open(jsonFilename_, ios::out);
	writingFile << json.dump() << endl;
	writingFile.close();
}

void Actor::Destroy(std::shared_ptr<Actor> actor)
{
	Engine::sceneManager_->GetActiveScene().DestroyActor(actor);
}

void AbyssEngine::Actor::OnCollision(const std::shared_ptr<Collider>& hitCollider, Collision::IntersectionResult result)
{
	//�X�N���v�g�R���|�[�l���g���p�����Ă���N���X��	OnCollision���Ă�
	for (const auto& c : componentList_)
	{
		const auto& sc = std::dynamic_pointer_cast<ScriptComponent>(c);
		if (sc)sc->OnCollision(hitCollider, result);
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
				RemoveParent();
			}

			//�e��ݒ�
			parent_ = parent;
			parent->children_.emplace_back(shared_from_this());
		}
	}
}

void Actor::RemoveParent()
{
	//�����Ɛe�̊֌W���ǂ��炩�������
	if (const auto& p = parent_.lock())
	{
		//�e�̎q�ꗗ���玩�g��T���A����
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
		//�e�̃I�u�W�F�N�g���A�N�e�B�u���ǂ������l������
		if (const auto& actor = parent_.lock())
		{
			return actor->GetActiveInHierarchy();
		}
	}
	return active_;
}

