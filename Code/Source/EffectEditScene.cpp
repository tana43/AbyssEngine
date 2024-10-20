#include "EffectEditScene.h"
#include "Camera.h"
#include "ComputeParticleEmitter.h"
#include "Engine.h"
#include "RenderManager.h"

using namespace AbyssEngine;

void EffectEditScene::Initialize()
{
    Scene::Initialize();

    //Engine::renderManager_->GetBufferScene().data_.
    const auto& camera = InstanceActor("DebugCamera");
    camera->AddComponent<AbyssEngine::Camera>();

    const auto& effectEmitter = InstanceActor("Effect");
    effectEmitter->AddComponent<ComputeParticleEmitter>();

}
