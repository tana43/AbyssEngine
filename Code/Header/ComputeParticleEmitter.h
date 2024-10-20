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

        //�p�[�e�B�N������
        void EmitParticle();

    private:
        //std::shared_ptr<ComputeParticleSystem> particleSystem_;
        std::shared_ptr<Texture> texture_;

        //�@��x�ɐ��������p�[�e�B�N���̐�
        int emitNum_ = 100;

        // �p�[�e�B�N������
        float lifespan_ = 1.0f;

        //�e�N�X�`���̃^�C�v 
        int texType_ = 0;

        // �����͈͂̎w��
        //Vector3 emitAreaMin_ = { 0,0,0 };
        //Vector3 emitAreaMax_ = { 1,1,1 };

        //�@�����ʒu�̐U�ꕝ
        Vector3 positionAmplitude_ = {};
        //�@�������x
        Vector3 velocity_ = {0.0f,10.0f,0.0f};
        //�@�������x�̐U�ꕝ
        Vector3 velocityAmplitude_ = {3.0f,1.0f,3.0f};
        //�@���������x
        Vector3 acceleration_ = {0.0f,-3.0f,0.0f};
        //�@���������x�̐U�ꕝ
        Vector3 accelerationAmplitud_ = {};

        //�@�����X�P�[���̐U�ꕝ
        Vector2 scaleAmplitude_ = {};
        //�@�����X�P�[�����x
        Vector2 scaleVelocity_ = {};
        //�@�����X�P�[�����x�̐U�ꕝ
        Vector2 scaleVelocityAmplitude_ = {};
        //�@�����X�P�[�������x
        Vector2 scaleAcceleration_ = {};
        //�@�����X�P�[�������x�̐U�ꕝ
        Vector2 scaleAccelerationAmplitud_ = {};

        //�@������]�̐U�ꕝ
        Vector3 rotationAmplitude_ = {};
        //�@������]���x
        Vector3 rotationVelocity_ = {};
        //�@������]���x�̐U�ꕝ
        Vector3 rotationVelocityAmplitude_ = {};
        //�@������]�����x
        Vector3 rotationAcceleration_ = {};
        //�@������]�����x�̐U�ꕝ
        Vector3 rotationAccelerationAmplitud_ = {};

        float brightness_ = 1.0f;//color�̏搔

        Vector4 color_ = {1,1,1,1};

        //�@�F�̐U�ꕝ
        Vector4 colorAmplitud_ = {1,1,1,1};

        //X�L�[���������ƂŐ����\�ɂ���
#if _DEBUG
        bool canKeyXEmit_ = true;
#else
        bool canKeyXEmit_ = false;
#endif // _DEBUG

    };
}


