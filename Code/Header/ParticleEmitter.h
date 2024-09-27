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
        void RecalculateFrame()override;
        void DrawImGui()override;

    private:
        //本体のパーティクルシステム
        std::unique_ptr<ParticleSystem> particleSystem_;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleTexture_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise3d_;
    };
}
