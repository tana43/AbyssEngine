#include "TestSceneSecond.h"
#include "Helicopter.h"
#include "Camera.h"
#include "BossMech.h"
#include "Vitesse.h"
#include "ParticleEmitter.h"
#include "ComputeParticleEmitter.h"
#include "TrailRenderer.h"

using namespace AbyssEngine;

void TestSceneSecond::Initialize()
{
    Scene::Initialize();

    const auto& camera = InstanceActor("DebugCamera_TestS");
    camera->AddComponent<AbyssEngine::Camera>();

    //const auto& heli = InstanceActor("Helicopter_TestS");
    //heli->AddComponent<Helicopter>();

    const auto& effectEmitter = InstanceActor("Effect");
    //effectEmitter->AddComponent<ParticleEmitter>();
    effectEmitter->AddComponent<ComputeParticleEmitter>();
    
    const auto& trail = InstanceActor("Trail");
    trail->AddComponent<TrailRenderer>("./Assets/Effects/TrailTexture/Beam.png");
}
