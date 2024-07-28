#pragma once
#include "Collider.h"
#include "MathHelper.h"

namespace AbyssEngine
{
    class SphereCollider : public Collider
    {
    public: 
        SphereCollider() {}
        ~SphereCollider() {}

        void Initialize(const std::shared_ptr<Actor>& actor)override;

        void DrawDebug();
    private:
        //���a
        float radius_;

        //�f�o�b�O���ɕ\������鋅�̂̃J���[
        Vector4 debugColor_ = { 1,1,1,1 };
    };
}

