#pragma once
#include "ScriptComponent.h"
#include "MathHelper.h"

namespace AbyssEngine
{
    class Projectile;
}

class GunComponent : public AbyssEngine::ScriptComponent
{
public:
    GunComponent() {}
    ~GunComponent() = default;

private:
    AbyssEngine::Vector3 muzzle_;

    
};

