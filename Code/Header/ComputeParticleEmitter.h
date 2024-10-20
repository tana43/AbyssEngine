#pragma once
#include "Renderer.h"
#include "ComputeParticleSystem.h"

namespace AbyssEngine
{
    class Texture;

    //シェーダーの授業ベースGPUパーティクル
    class ComputeParticleEmitter : public Renderer
    {
    public:
        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Render()override;
        void RecalculateFrame()override;
        void DrawImGui()override;

        void Update();

    private:
        //std::shared_ptr<ComputeParticleSystem> particleSystem_;
        std::shared_ptr<Texture> texture_;

        float lifespan_;//パーティクル寿命

        Vector4 color_ = {1,1,1,1};
    };
}


