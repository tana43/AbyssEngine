#include "ScriptComponentManager.h"

using namespace AbyssEngine;

void ScriptComponentManager::Update()
{
	int originalSize = scriptComList_.size();

	for (int index = 0; index < originalSize; ++index)
	{
		if (const auto& c = scriptComList_[index].lock())
		{
			if (!c->GetActive())continue;
			c->UpdateBegin();
		}
	}

	for (int index = 0; index < originalSize; ++index)
	{
		if (const auto& c = scriptComList_[index].lock())
		{
			if (!c->GetActive())continue;
			c->UpdateBefore();
		}
	}

	for (int index = 0; index < originalSize; ++index)
	{
		if (const auto& c = scriptComList_[index].lock())
		{
			if (!c->GetActive())continue;
			c->Update();
		}
	}

	for (int index = 0; index < originalSize; ++index)
	{
		if (const auto& c = scriptComList_[index].lock())
		{
			if (!c->GetActive())continue;
			c->UpdateAfter();
		}
	}

	for (int index = 0; index < originalSize; ++index)
	{
		if (const auto& c = scriptComList_[index].lock())
		{
			if (!c->GetActive())continue;
			c->UpdateEnd();
		}
	}
}

void AbyssEngine::ScriptComponentManager::AddScriptCom(const std::shared_ptr<ScriptComponent>& com)
{
	scriptComList_.emplace_back(com);
}

//void CharacterManager::DelayedUpdate()
//{
//	for (const auto& c : characterList_)
//	{
//		if (!c->GetIsActive())continue;
//
//		c->DelayedUpdate();
//	}
//}

void ScriptComponentManager::Clear()
{
	scriptComList_.clear();
}
