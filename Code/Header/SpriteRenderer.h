#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "Renderer.h"

namespace AbyssEngine
{
    class Texture;
    class Material;

    //�X�v���C�g�����_���[�R���|�[�l���g
    class SpriteRenderer : public Renderer
    {
    private:
        struct Vertex
        {
            Vector3 pos_;     //�ʒu
            Vector2 tex_;     //UV�ʒu
            Vector4 color_;   //���_�F
        };

    private:
        //void Initialize(const std::shared_ptr<GameObject>& obj)override;

    public:
        //������
        void Initialize();
        void Render();      //�`����s
        void RecalculateFrame();//�\�����W���v�Z����

    private:
        Vector2 size_ = { 100,100 };   //�\���T�C�Y
        Vector2 uvOrigin_ = { 0,0 };   //�e�N�X�`����UV���_
        Vector2 uvSize_ = { 100,100 }; //�e�N�X�`������؂�o���T�C�Y�iUV���_��j
        Vector4 color_ = { 1,1,1,1 };  //�e�N�X�`���J���[
        std::shared_ptr<Texture> texture_;       //�`�悷��e�N�X�`��

        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;

        std::string fileName_;
        std::string filePath_;
        
        bool visibility_ = false;//�\���؂�ւ�
    };
}


