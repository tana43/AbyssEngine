#include "GameCollider.h"

#include "Actor.h"

#include "Engine.h"
#include "CollisionManager.h"

using namespace AbyssEngine;

AttackCollider::~AttackCollider()
{
    const auto& p = dynamic_pointer_cast<SphereCollider>(shared_from_this());
    if (p)
    {
        Engine::collisionManager_->DeleteAttackCollider(p);
    }
    else
    {
        _ASSERT_EXPR(false,L"�L���X�g�Ɏ��s");
    }
}

void AttackCollider::Initialize(const std::shared_ptr<Actor>& actor)
{
    SphereCollider::Initialize(actor);

    //�}�l�[�W���[�ɓo�^
    Engine::collisionManager_->RegisterAttackCollider(static_pointer_cast<SphereCollider>(shared_from_this()));

    //�f�o�b�O���̃J���[�ύX
    debugColor_ = Vector4(1, 0, 0, 1);
}

HitCollider::~HitCollider()
{
    Engine::collisionManager_->DeleteHitCollider(static_pointer_cast<SphereCollider>(shared_from_this()));
}

void HitCollider::Initialize(const std::shared_ptr<Actor>& actor)
{
    SphereCollider::Initialize(actor);

    //�}�l�[�W���[�ɓo�^
    Engine::collisionManager_->RegisterHitCollider(static_pointer_cast<SphereCollider>(shared_from_this()));

    //�f�o�b�O���̃J���[�ύX
    debugColor_ = Vector4(1, 0, 1, 1);
}
