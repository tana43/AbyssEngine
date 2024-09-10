#include "ScriptComponent.h"
#include "Actor.h"
#include "Engine.h"
#include "ScriptComponentManager.h"
#include "SphereCollider.h"
#include "SceneManager.h"
#include "CollisionManager.h"

using namespace AbyssEngine;

void ScriptComponent::Initialize(const std::shared_ptr<Actor>& actor)
{
    Engine::scriptComManager_->AddScriptCom(std::static_pointer_cast<ScriptComponent>(shared_from_this()));
    actor_ = actor;
    transform_ = actor->GetTransform();
}

std::shared_ptr<SphereCollider> ScriptComponent::AddAttackCollider(AbyssEngine::Vector3 localPos, float radius, std::string name)
{
    //�R���C�_�[�p�̃A�N�^�[�𐶐�
    auto& scene = Engine::sceneManager_->GetActiveScene();
    const auto& colliderActor = scene.InstanceActor(name);

    const auto& colliderCom = colliderActor->AddComponent<SphereCollider>();
    colliderCom->SetRadius(radius);

    //�ʒu�ύX
    colliderActor->GetTransform()->SetLocalPosition(localPos);

    //�e�q�t��
    colliderActor->SetParent(actor_);

    //�}�l�[�W���[�ɓo�^����
    Engine::collisionManager_->RegisterAttackCollider(colliderCom);

    //�f�o�b�O���̃J���[�ύX
    colliderCom->SetDebugColor(Vector4(1, 0, 0, 1));

    return colliderCom;
}

std::shared_ptr<SphereCollider> ScriptComponent::AddHitCollider(Vector3 localPos, float radius, std::string name)
{
    //�R���C�_�[�p�̃A�N�^�[�𐶐�
    auto& scene = Engine::sceneManager_->GetActiveScene();
    const auto& colliderActor = scene.InstanceActor(name);

    const auto& colliderCom = colliderActor->AddComponent<SphereCollider>();
    colliderCom->SetRadius(radius);

    //�ʒu�ύX
    colliderActor->GetTransform()->SetLocalPosition(localPos);

    //�e�q�t��
    colliderActor->SetParent(actor_);

    //�}�l�[�W���[�ɓo�^����
    Engine::collisionManager_->RegisterHitCollider(colliderCom);
    
    //�f�o�b�O���̃J���[�ύX
    colliderCom->SetDebugColor(Vector4(1, 0, 1, 1));

    return colliderCom;
}
