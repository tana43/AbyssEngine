#pragma once
#include "Renderer.h"
#include <d3d11.h>
#include <wrl.h>

#include "ConstantBuffer.h"

namespace AbyssEngine
{
    class Texture;

    //�e�N�X�`���t���̔|��
    class BillboardRenderer : public Renderer
    {
    public:
        void Initialize(const std::shared_ptr<Actor>& actor)override;

        //�s��Ȃǂ̌v�Z
        void RecalculateFrame()override;

        //�`��
        void Render()override;

        bool DrawImGui()override;

    private:
        void SetActive(bool value)override;//�A�N�e�B�u��Ԃ�؂�ւ���
    public:
        struct Constants
        {
            Matrix worldMatrix_;
            Vector4 color_ = {1,1,1,1};
        };

    public:
        void SetVisibility(const bool& flag) { visibility_ = flag; }

        const std::unique_ptr<ConstantBuffer<Constants>>& GetConstantBuffer() { return constantBuffer_; }

    private:
        // ���_�\����
        struct Vertex 
        {
            Vector3 position_;
            Vector2 texcoord_;
        };

    private:
        Vector2 pivot_;//���S�_
        Vector2 cropSize_;//�e�N�X�`���̐؂���T�C�Y

        std::shared_ptr<Texture> texture_; //�e�N�X�`��

        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer_;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;

        bool billboard_ = true;//�r���{�[�h�I���E�I�t

        bool visibility_ = true;//�\���E��\��

        //�萔�o�b�t�@
        std::unique_ptr<ConstantBuffer<Constants>> constantBuffer_;
    };
}