#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "Renderer.h"

namespace AbyssEngine
{
    class Texture;

    //�X�v���C�g�����_���[�R���|�[�l���g
    class SpriteRenderer : public Renderer
    {
    public:
        SpriteRenderer() {}
        ~SpriteRenderer() {}

        //����alpha�̒l�փt�F�[�h�C���E�A�E�g���� �t�F�[�h��������True��Ԃ�
        bool FadeIn(float alpha, float changeSpeed);
        bool FadeOut(float alpha, float changeSpeed);

    private:
        struct Vertex
        {
            Vector3 pos_;     //�ʒu
            Vector2 tex_;     //UV�ʒu
            Vector4 color_;   //���_�F
        };

    public:
        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Render()override;      //�`����s
        void RecalculateFrame();//�\�����W���v�Z����

        void DrawImGui()override;

    public:
        void SetColor(const Vector4& color) { color_ = color; }
        const Vector4& GetColor() const { return color_; }

        void SetColorAlpha(const float alpha) { color_.w = alpha; }
        const float& GetColorAlpha() const { return color_.w; }

        void SetSize(const Vector2& size) { size_ = size; }
        const Vector2& GetSize() const { return size_; }

        void SetOffsetPosition(const Vector2& pos) { offsetPos_ = pos; }
        const Vector2& GetOffsetPosition() const { return offsetPos_; }

        void SetPivot(const Vector2& pivot) { pivot_ = pivot; }
        const Vector2& GetPivot() const { return pivot_; }

        void SetScale(const Vector2& sca) { scale_ = sca; }
        const Vector2& GetScale() const { return scale_; }

        void SetAngle(const float& angle) { angle_ = angle; }
        const float& GetAngle() const { return angle_; }

    private:
        void SetActive(const bool value)override;//�A�N�e�B�u��Ԃ�ݒ肷��
    private:
        Vector2 size_ = { 100,100 };   //�\���T�C�Y
        Vector2 scale_ = { 1,1 };   //�X�P�[��
        Vector2 uvOrigin_ = { 0,0 };   //�e�N�X�`����UV���_
        Vector2 uvSize_ = { 100,100 }; //�e�N�X�`������؂�o���T�C�Y�iUV���_��j
        Vector4 color_ = { 1,1,1,1 };  //�e�N�X�`���J���[
        Vector2 pivot_ = { 0,0 };//���S�_
        Vector2 offsetPos_ = { 0,0 };//�I�t�Z�b�g���W
        float angle_ = 0.0f; //�p�x
        std::shared_ptr<Texture> texture_;       //�`�悷��e�N�X�`��

        Microsoft::WRL::ComPtr<ID3D11Buffer>        vertexBuffer_;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>   pixelShader_;
        Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout_;
        
        bool visibility_ = true;//�\���؂�ւ�(true�Ȃ�\���Afalse�Ȃ��\��)
    };
}


