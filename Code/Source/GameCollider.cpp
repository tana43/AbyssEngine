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
        _ASSERT_EXPR(false,L"キャストに失敗");
    }
}

void AttackCollider::Initialize(const std::shared_ptr<Actor>& actor)
{
    SphereCollider::Initialize(actor);

    //マネージャーに登録
    Engine::collisionManager_->RegisterAttackCollider(static_pointer_cast<SphereCollider>(shared_from_this()));

    //デバッグ球のカラー変更
    debugColor_ = Vector4(1, 0, 0, 1);
}

HitCollider::~HitCollider()
{
    Engine::collisionManager_->DeleteHitCollider(static_pointer_cast<SphereCollider>(shared_from_this()));
}

void HitCollider::Initialize(const std::shared_ptr<Actor>& actor)
{
    SphereCollider::Initialize(actor);

    //マネージャーに登録
    Engine::collisionManager_->RegisterHitCollider(static_pointer_cast<SphereCollider>(shared_from_this()));

    //デバッグ球のカラー変更
    debugColor_ = Vector4(1, 0, 1, 1);
}
