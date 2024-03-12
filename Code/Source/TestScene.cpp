#include "TestScene.h"
#include "SpriteRenderer.h"

using namespace AbyssEngine;

void TestScene::Initialize()
{
    auto p = InstanceActor("testObj");
    p->AddComponent<SpriteRenderer>();
}

void TestScene::Update()
{

}
