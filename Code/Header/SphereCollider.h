#pragma once
#include "Collider.h"

namespace AbyssEngine
{
    class SphereCollider : public Collider
    {
    public: 
        SphereCollider() {}
        ~SphereCollider() {}

        void Initialize(const std::shared_ptr<Actor>& actor)override;

        void DrawDebug();

        void DrawImGui()override;

        bool IntersectVsSphere(const std::shared_ptr<SphereCollider>& collider, Collision::IntersectionResult* result);

    public:
        const float& GetRadius() const { return radius_; }
        void SetRadius(const float& radius) { radius_ = radius; }

        void SetDebugColor(const Vector4& color) { debugColor_ = color; }

    private:
        //���a
        float radius_;

        //�f�o�b�O���ɕ\������鋅�̂̃J���[
        Vector4 debugColor_ = { 1,0,1,1 };
    };
}

