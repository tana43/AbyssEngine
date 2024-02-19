#pragma once
#include <DirectXMath.h>
#include <memory>
#include <wrl.h>
#include <d3d11.h>
#include <string>

namespace AbyssEngine
{
    class Texture;
    class Material;

    //�X�v���C�g�����_���[�R���|�[�l���g
    class SpriteRenderer : public 
    {
    private:
        struct Vertex
        {
            DirectX::XMFLOAT3 pos_;     //�ʒu
            DirectX::XMFLOAT2 tex_;     //UV�ʒu
            DirectX::XMFLOAT4 color_;   //���_�F
        };

    private:
        //void Initialize(const std::shared_ptr<GameObject>& obj)override;

    public:
        //������
        void Initialize();
        void Render();      //�`����s
        void RecalculateFrame();//�\�����W���v�Z����

    private:
        DirectX::XMFLOAT2 size_ = { 100,100 };   //�\���T�C�Y
        DirectX::XMFLOAT2 uvOrigin_ = { 0,0 };   //�e�N�X�`����UV���_
        DirectX::XMFLOAT2 uvSize_ = { 100,100 }; //�e�N�X�`������؂�o���T�C�Y�iUV���_��j
        DirectX::XMFLOAT4 color_ = { 1,1,1,1 };  //�e�N�X�`���J���[
        std::shared_ptr<Texture> texture_;       //�`�悷��e�N�X�`��

        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;

        std::string fileName_;
        std::string filePath_;
        
        bool visibility_ = false;//�\���؂�ւ�
    };
}


