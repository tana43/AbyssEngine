#include "ScriptComponent.h"
#include "Actor.h"
#include "Engine.h"
#include "ScriptComponentManager.h"
#include "SphereCollider.h"
#include "SceneManager.h"
#include "CollisionManager.h"
#include "GameCollider.h"

using namespace AbyssEngine;

void ScriptComponent::Initialize(const std::shared_ptr<Actor>& actor)
{
    Engine::scriptComManager_->AddScriptCom(std::static_pointer_cast<ScriptComponent>(shared_from_this()));
    actor_ = actor;
    transform_ = actor->GetTransform();
}

std::shared_ptr<SphereCollider> ScriptComponent::AddAttackCollider(AbyssEngine::Vector3 localPos, float radius, std::string name)
{
    //コライダー用のアクターを生成
    auto& scene = Engine::sceneManager_->GetActiveScene();
    const auto& colliderActor = scene.InstanceActor(name);

    const auto& colliderCom = colliderActor->AddComponent<AttackCollider>();
    colliderCom->SetRadius(radius);

    //位置変更
    colliderActor->GetTransform()->SetLocalPosition(localPos);

    //親子付け
    colliderActor->SetParent(actor_);

    //デバッグ球のカラー変更
    colliderCom->SetDebugColor(Vector4(1, 0, 0, 1));

    return colliderCom;
}

std::shared_ptr<SphereCollider> ScriptComponent::AddHitCollider(Vector3 localPos, float radius, std::string name)
{
    //コライダー用のアクターを生成
    auto& scene = Engine::sceneManager_->GetActiveScene();
    const auto& colliderActor = scene.InstanceActor(name);

    const auto& colliderCom = colliderActor->AddComponent<HitCollider>();
    colliderCom->SetRadius(radius);

    //位置変更
    colliderActor->GetTransform()->SetLocalPosition(localPos);

    //親子付け
    colliderActor->SetParent(actor_);

    return colliderCom;
}
