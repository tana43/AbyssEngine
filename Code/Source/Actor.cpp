#include "Actor.h"
#include "Engine.h"

using namespace AbyssEngine;
using namespace std;

void Actor::Initialize()
{
    //�e�R���|�[�l���g�̏��������Ă�
	for (const auto& c : componentList_)
	{
		c->Initialize(dynamic_pointer_cast<Actor>())
	}
}
