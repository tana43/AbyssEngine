#pragma once

#include <vector>
#include "MathHelper.h"
#include "Renderer.h"

namespace AbyssEngine
{
    class SpriteRenderer;
    class SkeletalMesh;
    class GltfSkeletalMesh;
    class StaticMesh;
    class Camera;
    class Bloom;
    class FrameBuffer;
    class FullscreenQuad;

    class RenderManager
    {
    private:
        enum class SP_State {
            Point_Wrap,Linear_Wrap,Anisotropic_Wrap,
            Point_Clamp,Linear_Clamp, Anisotropic_Clamp,
            Point_Border_Opaque_Black, Linear_Border_Opaque_Black, Point_Border_Opaque_White, Linear_Border_Opaque_White,
            Comparision_Depth,Count
        };

    public:
        RenderManager();

        void Reset();
        void Add(const std::shared_ptr<SpriteRenderer>& mRend);//�}�l�[�W���[�Ƀ����_���[��o�^����
        void Add(const std::shared_ptr<SkeletalMesh>& mRend);//�}�l�[�W���[�Ƀ����_���[��o�^����
        //void Add(const std::shared_ptr<GltfSkeletalMesh>& mRend);//�}�l�[�W���[�Ƀ����_���[��o�^����
        void Add(const std::shared_ptr<StaticMesh>& mRend);//�}�l�[�W���[�Ƀ����_���[��o�^����
        void Add(const std::shared_ptr<Camera>& camera_);//�}�l�[�W���[�ɃJ������o�^����

        void Render(); //�`����s

        void DrawImGui();

        //bool renderGame_ = true; //�Q�[���r���[��`�悷��
        //bool renderScene_ = true; //�V�[���r���[��`�悷��

    private:
        struct Vertex
        {
            Vector3 pos_;
            Vector2 tex_;
            Vector4 color_;
        };

        Microsoft::WRL::ComPtr<ID3D11SamplerState> samplers_[static_cast<int>(SP_State::Count)];


    private:
        std::vector<std::weak_ptr<Renderer>> renderer3DList_{};
        std::vector<std::weak_ptr<Renderer>> renderer2DList_{};
        
        std::vector<std::weak_ptr<Camera>> cameraList_{};

        //���͍Œ���K�v�Ȃ��̂����p��
        struct ConstantBufferScene
        {
            Matrix view_;
            Matrix projection_;
            Matrix viewProjectionMatrix_;
            Matrix inverseViewProjection_;
            //Matrix shadowMatrix_;
            //Vector4 cameraDirection_;
            Vector4 lightDirection_ = {0,0,1,0};
            Vector4 eyePosition_;
            Vector4 focusPosition_;
            //Vector3 lightColor_;
            float exposure_ = 1.0f;
            float pureWhite_ = 3.0f;
            float emissiveIntensity_ = 50.0f;
            float imageBasedLightingIntensity_ = 1.0f;
            float skyboxRoughness_ = 1.0f;
            float time_ = 0.0f;
        };

        ConstantBufferScene bufferScene_;
        Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferScene_;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> iblShaderResourceView_[4];

        //�I�t�X�N���[�������_�����O
        std::unique_ptr<FrameBuffer> frameBuffer_;
        std::unique_ptr<FullscreenQuad> bitBlockTransfer_;
        std::unique_ptr<Bloom> bloom_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;

    private:

        //2D�I�u�W�F�N�g�̃����_�����O
        void Render2D() const;

        void Render3D(const std::shared_ptr<Camera>& camera_);

        //Renderer�̐����m�F
        void CheckRenderer();

        void UpdateConstantBuffer()const;

        void IBLInitialize();
        void IBLSetResources();
    };

}

