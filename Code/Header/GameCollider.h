#pragma once
#include "SphereCollider.h"

namespace AbyssEngine
{
    //�U������R���C�_�[
    class AttackCollider : public SphereCollider
    {
    public:
        AttackCollider() {}
        ~AttackCollider();

        void Initialize(const std::shared_ptr<Actor>& actor)override;
    };
    
    //��炢����R���C�_�[
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
        //���b�N�I���̓I�ɂȂ邩
        bool lockonTarget_ = false;
    };

    //�n�`(�����o��)����R���C�_�[
    class TerrainCollider : public SphereCollider
    {
    public:
        TerrainCollider() {}
        ~TerrainCollider();

        void Initialize(const std::shared_ptr<Actor>& actor)override;
    };
}


