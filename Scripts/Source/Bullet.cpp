#include "Bullet.h"
#include "Actor.h"
#include "StaticMesh.h"

using namespace AbyssEngine;

void Bullet::Initialize(const std::shared_ptr<Actor>& actor)
{
    Projectile::Initialize(actor);

    actor->AddComponent<StaticMesh>("./Assets/Models/Cube.glb");
    transform_->SetScaleFactor(0.1f);
}

void Bullet::Update()
{
    Projectile::Update();
}
