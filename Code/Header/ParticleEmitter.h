#pragma once
#include "Renderer.h"
#include "Particles.h"

namespace AbyssEngine
{
    //パーティクルを管理させるコンポーネント
    class ParticleEmitter : public Renderer
    {
    public:
        ParticleEmitter() {}
        ~ParticleEmitter() {}

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Render()override;



    private:
        //本体のパーティクルシステム
        std::unique_ptr<Particle> particleSystem_;

    };
}
