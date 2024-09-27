#include "TestSceneSecond.h"
#include "Helicopter.h"
#include "Camera.h"
#include "BossMech.h"
#include "Vitesse.h"
#include "ParticleEmitter.h"

using namespace AbyssEngine;

void TestSceneSecond::Initialize()
{
    Scene::Initialize();

    const auto& camera = InstanceActor("DebugCamera_TestS");
    camera->AddComponent<AbyssEngine::Camera>();

    const auto& heli = InstanceActor("Helicopter_TestS");
    heli->AddComponent<Helicopter>();

    const auto& effectEmitter = InstanceActor("Effect");
    effectEmitter->AddComponent<ParticleEmitter>();

    
}
