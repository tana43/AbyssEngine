#include "CharacterManager.h"

using namespace AbyssEngine;

void CharacterManager::Update()
{
	for (const auto& c : characterList_)
	{
		if (!c->GetIsActive())continue;

		c->Update();
	}
}

void CharacterManager::AddCharacter(const std::shared_ptr<Character>& chara)
{
	characterList_.emplace_back(chara);
}

void CharacterManager::Clear()
{
	characterList_.clear();
}
