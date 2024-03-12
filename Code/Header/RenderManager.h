#pragma once

#include <vector>
#include "MathHelper.h"

namespace AbyssEngine
{
    class Renderer;
    class SpriteRender;

    class RenderManager
    {
    public:
        RenderManager();

        void Reset();
        void Add(const std::shared_ptr<SpriteRender>& mRend);//�}�l�[�W���[�Ƀ����_���[�o�^����

        void Render(); //�`����s

        //bool renderGame_ = true; //�Q�[���r���[��`�悷��
        //bool renderScene_ = true; //�V�[���r���[��`�悷��

    private:
        struct Vertex
        {
            Vector3 pos_;
            Vector2 tex_;
            Vector4 color_;
        };


    private:
        //std::vector<std::weak_ptr<Renderer>> renderer3DList_{};
        std::vector<std::weak_ptr<Renderer>> renderer2DList_{};

        //void RenderMain();

        //2D�I�u�W�F�N�g�̃����_�����O
        void Render2D() const;

        //Renderer�̐����m�F
        void CheckRenderer();
    };

}

