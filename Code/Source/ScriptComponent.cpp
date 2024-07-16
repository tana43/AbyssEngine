#include "ScriptComponent.h"
#include "Actor.h"
#include "Engine.h"
#include "ScriptComponentManager.h"

using namespace AbyssEngine;

void AbyssEngine::ScriptComponent::Initialize(const std::shared_ptr<Actor>& actor)
{
    Engine::scriptComManager_->AddScriptCom(std::static_pointer_cast<ScriptComponent>(shared_from_this()));
    actor_ = actor;
    transform_ = actor->GetTransform();
}
