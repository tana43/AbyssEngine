#include "Actor.h"
#include "Engine.h"

using namespace AbyssEngine;
using namespace std;

void Actor::Initialize()
{
    //各コンポーネントの初期化を呼ぶ
	for (const auto& c : componentList_)
	{
		c->Initialize(dynamic_pointer_cast<Actor>())
	}
}
