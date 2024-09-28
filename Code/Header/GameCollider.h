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

    public:
        const bool& GetLockonTarget() const { return lockonTarget_; }
        void SetLockonTarget(const bool& active) { lockonTarget_ = active; }

    private:
        //ロックオンの的になるか
        bool lockonTarget_ = false;
    };
}


