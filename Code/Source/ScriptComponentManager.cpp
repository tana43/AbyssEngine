#include "ScriptComponentManager.h"

using namespace AbyssEngine;

void ScriptComponentManager::Update()
{
	for (const auto& c : scriptComList_)
	{
		if (!c->GetIsActive())continue;

		c->UpdateBegin();
	}

	for (const auto& c : scriptComList_)
	{
		if (!c->GetIsActive())continue;

		c->UpdateBefore();
	}

	for (const auto& c : scriptComList_)
	{
		if (!c->GetIsActive())continue;

		c->Update();
	}

	for (const auto& c : scriptComList_)
	{
		if (!c->GetIsActive())continue;

		c->UpdateAfter();
	}

	for (const auto& c : scriptComList_)
	{
		if (!c->GetIsActive())continue;

		c->UpdateEnd();
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
