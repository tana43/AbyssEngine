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
        //半径
        float radius_;

        //デバッグ時に表示される球体のカラー
        Vector4 debugColor_ = { 1,1,1,1 };
    };
}

