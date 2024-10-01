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
		//ImGui表示
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
		//デバッグ表示
		c->DrawDebug();
	}
}

nlohmann::json Actor::ReadAndCreateJsonFile()
{
	//ファイルの読み込み
	ifstream ifs(jsonFilename_);
	if (ifs.good())
	{
		//ファイル読み込み
		nlohmann::json mJson;
		ifs >> mJson;
		return mJson;
	}
	else
	{
		//なければ作成して、空のjsonを返す
		ofstream writingFile;
		writingFile.open(jsonFilename_, ios::out);
		writingFile.close();

		return nlohmann::json();
	}
}

nlohmann::json AbyssEngine::Actor::ReadingJsonFile(bool& exist)
{
	//ファイル存在フラグをTrueに
	exist = true;

	//ファイルの読み込み
	ifstream ifs(jsonFilename_);
	if (ifs.good())
	{
		//ファイル読み込み
		nlohmann::json mJson;
		ifs >> mJson;
		return mJson;
	}
	else
	{
		//存在フラグをfalseにし、空のjsonを返す
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
	//スクリプトコンポーネントを継承しているクラスの	OnCollisionを呼ぶ
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

