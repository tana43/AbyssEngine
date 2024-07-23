#pragma once
#include "ScriptComponent.h"
#include "MathHelper.h"

namespace AbyssEngine
{
    class Projectile;
}

class Gun : public AbyssEngine::ScriptComponent
{
public:
    Gun() {}
    ~Gun() = default;


    void Shot();
private:
    AbyssEngine::Vector3 muzzle_;

    
};

