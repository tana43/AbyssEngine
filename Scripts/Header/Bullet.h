#pragma once
#include "Projectile.h"

class Bullet : public AbyssEngine::Projectile
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    void Update()override;
};

