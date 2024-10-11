#include "ScriptComponent.h"
#include "Actor.h"
#include "Engine.h"
#include "ScriptComponentManager.h"
#include "SphereCollider.h"
#include "SceneManager.h"
#include "CollisionManager.h"
#include "GameCollider.h"
#include "SkeletalMesh.h"

using namespace AbyssEngine;

void ScriptComponent::Initialize(const std::shared_ptr<Actor>& actor)
{
    Engine::scriptComManager_->AddScriptCom(std::static_pointer_cast<ScriptComponent>(shared_from_this()));
    actor_ = actor;
    transform_ = actor->GetTransform();
}

std::shared_ptr<TerrainCollider> ScriptComponent::AddTerrainCollider(Vector3 localPos, float radius, std::string name, const std::shared_ptr<SkeletalMesh>& attachModel, std::string socketName)
{
    //�R���C�_�[�p�̃A�N�^�[�𐶐�
    auto& scene = Engine::sceneManager_->GetActiveScene();
    const auto& colliderActor = scene.InstanceActor(name);

    const auto& colliderCom = colliderActor->AddComponent<TerrainCollider>();

    //���W�┼�a�̐ݒ�
    AddColliderCommon(colliderCom, localPos, radius, attachModel, socketName);

    return colliderCom;
}

std::shared_ptr<AttackCollider> ScriptComponent::AddAttackCollider(AbyssEngine::Vector3 localPos, float radius, std::string name,const std::shared_ptr<SkeletalMesh>& attachModel, std::string socketName)
{
    //�R���C�_�[�p�̃A�N�^�[�𐶐�
    auto& scene = Engine::sceneManager_->GetActiveScene();
    const auto& colliderActor = scene.InstanceActor(name);

    const auto& colliderCom = colliderActor->AddComponent<AttackCollider>();

    //���W�┼�a�̐ݒ�
    AddColliderCommon(colliderCom, localPos, radius, attachModel, socketName);

    return colliderCom;
}

std::shared_ptr<HitCollider> ScriptComponent::AddHitCollider(Vector3 localPos, float radius, std::string name, const std::shared_ptr<SkeletalMesh>& attachModel,std::string socketName)
{
    //�R���C�_�[�p�̃A�N�^�[�𐶐�
    auto& scene = Engine::sceneManager_->GetActiveScene();
    const auto& colliderActor = scene.InstanceActor(name);

    const auto& colliderCom = colliderActor->AddComponent<HitCollider>();
    
    //���W�┼�a�̐ݒ�
    AddColliderCommon(colliderCom,localPos,radius,attachModel,socketName);

    return colliderCom;
}

void AbyssEngine::ScriptComponent::AddColliderCommon(const std::shared_ptr<SphereCollider>& colliderCom,Vector3 localPos, float radius, const std::shared_ptr<SkeletalMesh>& attachModel, std::string socketName)
{
    colliderCom->SetRadius(radius);
    const auto& colliderActor = colliderCom->GetActor();

    //�ʒu�ύX
    colliderActor->GetTransform()->SetLocalPosition(localPos);

    //�e�q�t��
    colliderActor->SetParent(actor_);

    //�A�^�b�`�����郂�f��������Ȃ�A�^�b�`����
    if (attachModel)
    {
        colliderCom->AttachModel(attachModel, socketName);
    }
}
