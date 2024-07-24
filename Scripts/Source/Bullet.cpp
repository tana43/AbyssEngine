#include "Bullet.h"
#include "Actor.h"

using namespace AbyssEngine;

void Bullet::Initialize(const std::shared_ptr<Actor>& actor)
{
    Projectile::Initialize(actor);
}

void Bullet::Update()
{
    Projectile::Update();
}
