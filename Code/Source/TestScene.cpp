#include "TestScene.h"
#include "SpriteRenderer.h"

using namespace AbyssEngine;

void TestScene::Initialize()
{
    auto p = InstanceActor("testObj");
    auto sr = p->AddComponent<SpriteRenderer>("./Assets/Images/Background_with_line_wave_pattern_5.jpg");
}

void TestScene::Update()
{

}
