#pragma once
#include "Renderer.h"
#include "Particles.h"

namespace AbyssEngine
{
    //�p�[�e�B�N�����Ǘ�������R���|�[�l���g
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
        //�{�̂̃p�[�e�B�N���V�X�e��
        std::unique_ptr<ParticleSystem> particleSystem_;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleTexture_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise3d_;
    };
}
