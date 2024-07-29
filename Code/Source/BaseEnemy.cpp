#include "BaseEnemy.h"
#include  "Actor.h"

using namespace AbyssEngine;

void BaseEnemy::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //������
    Character::Initialize(actor);

    //�G�l�~�[�^�O�t��
    Character::ReplaceTag(Character::Tag_Enemy);
}
