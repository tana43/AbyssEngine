#include "TestScene.h"
#include "SpriteRenderer.h"

using namespace AbyssEngine;

void TestScene::Initialize()
{
    auto p = InstanceActor("testObj");
    auto sr = p->AddComponent<SpriteRenderer>("./Assets/Images/image_00.png");
}

void TestScene::Update()
{

}
