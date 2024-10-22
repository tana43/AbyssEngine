#include "EffectEditScene.h"
#include "Camera.h"
#include "ComputeParticleEmitter.h"
#include "Engine.h"
#include "RenderManager.h"

#include "imgui/imgui_neo_internal.h"
#include "imgui/imgui_neo_sequencer.h"

using namespace AbyssEngine;

void EffectEditScene::Initialize()
{
    Scene::Initialize();

    //Engine::renderManager_->GetBufferScene().data_.
    const auto& camera = InstanceActor("DebugCamera");
    camera->AddComponent<AbyssEngine::Camera>();

    const auto& effectEmitter = InstanceActor("Effect");
    effectEmitter->AddComponent<ComputeParticleEmitter>();
    effectEmitter->imguiAlways_ = true;

}

void EffectEditScene::DrawImGuiAlways()
{
    Scene::DrawImGuiAlways();

}
