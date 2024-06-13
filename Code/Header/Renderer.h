#pragma once
#include "Component.h"
#include "MathHelper.h"
#include "Shader.h"
#include <DirectXCollision.h>

namespace AbyssEngine
{
    //�����_�����O�����R���|�[�l���g�̊�b�N���X
    class Renderer : public Component 
    {
    public:
        Renderer() {}
        ~Renderer() {}

    public:
        void SetFilePath(const char* path) { filePath_ = path; }

        void SetEnable(bool value);
        [[nodiscard]] bool GetEnabled() const { return enabled_; }

    protected:
        bool isCalled_ = false;     //���Ƀ}�l�[�W���[�ɓo�^����Ă��邩
        bool enabled_ = true;       //�`�悷�邩
        bool canRender_ = false;    //�����_�����O�\���i�f�[�^�������Ă��Ȃ���)

        std::string filePath_;

        //int subsetCount_ = 0; //�T�u�Z�b�g�̐�
    private:
        //virtual void Render(const int subsetNumber = 0) {}          //�`����s
        virtual void Render() = 0;          //�`����s
        virtual void RenderShadow() {}    //�V���h�E�}�b�v�`����s
        virtual void RecalculateFrame() {}                         //���_�f�[�^���Čv�Z
        virtual bool FrustumCulling(const DirectX::BoundingFrustum& frustum) { return true; }//�t���X�^���J�����O

        friend class RenderManager;
    };
}


