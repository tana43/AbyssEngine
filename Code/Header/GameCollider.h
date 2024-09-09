#pragma once
#include "SphereCollider.h"

namespace AbyssEngine
{
    class AttackCollider : public SphereCollider
    {
    public:
        void Initialize(const std::shared_ptr<Actor>& actor)override;
    };
}


