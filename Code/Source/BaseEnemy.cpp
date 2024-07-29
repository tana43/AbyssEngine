#include "BaseEnemy.h"
#include  "Actor.h"

using namespace AbyssEngine;

void BaseEnemy::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //初期化
    Character::Initialize(actor);

    //エネミータグ付け
    Character::ReplaceTag(Character::Tag_Enemy);
}
