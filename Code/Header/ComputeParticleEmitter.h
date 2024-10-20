#pragma once
#include "Renderer.h"
#include "ComputeParticleSystem.h"

namespace AbyssEngine
{
    class Texture;

    //�V�F�[�_�[�̎��ƃx�[�XGPU�p�[�e�B�N��
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

        float lifespan_;//�p�[�e�B�N������

        Vector4 color_ = {1,1,1,1};
    };
}


