#include "TestSceneSecond.h"
#include "Helicopter.h"
#include "Camera.h"

using namespace AbyssEngine;

void TestSceneSecond::Initialize()
{
    const auto& camera = InstanceActor("DebugCamera_TestS");
    camera->AddComponent<AbyssEngine::Camera>();

    const auto& heli = InstanceActor("Helicopter_TestS");
    heli->AddComponent<Helicopter>();
}
