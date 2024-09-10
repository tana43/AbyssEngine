#pragma once
#include "SphereCollider.h"

namespace AbyssEngine
{
    class AttackCollider : public SphereCollider
    {
    public:
        AttackCollider() {}
        ~AttackCollider();

        void Initialize(const std::shared_ptr<Actor>& actor)override;
    };

    class HitCollider : public SphereCollider
    {
    public:
        HitCollider() {}
        ~HitCollider();

        void Initialize(const std::shared_ptr<Actor>& actor)override;
    };
}


