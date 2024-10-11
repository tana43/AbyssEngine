#pragma once
#include "SphereCollider.h"

namespace AbyssEngine
{
    //攻撃判定コライダー
    class AttackCollider : public SphereCollider
    {
    public:
        AttackCollider() {}
        ~AttackCollider();

        void Initialize(const std::shared_ptr<Actor>& actor)override;
    };
    
    //喰らい判定コライダー
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

    //地形(押し出し)判定コライダー
    class TerrainCollider : public SphereCollider
    {
    public:
        TerrainCollider() {}
        ~TerrainCollider();

        void Initialize(const std::shared_ptr<Actor>& actor)override;
    };
}


