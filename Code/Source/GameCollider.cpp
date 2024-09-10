#include "GameCollider.h"

#include "Actor.h"

#include "Engine.h"
#include "CollisionManager.h"

using namespace AbyssEngine;

void AttackCollider::Initialize(const std::shared_ptr<Actor>& actor)
{
    SphereCollider::Initialize(actor);

    //�}�l�[�W���[�ɓo�^
    Engine::collisionManager_->RegisterAttackCollider(static_pointer_cast<SphereCollider>(shared_from_this()));
}
