#include "Projectile.h"
#include "Actor.h"

void Projectile::Initialize(const std::shared_ptr<AbyssEngine::Actor> actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();
}

void Projectile::Update()
{
}
