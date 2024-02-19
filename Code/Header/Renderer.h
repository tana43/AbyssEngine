#pragma once
#include "Component.h"

namespace AbyssEngine
{
    //�����_�����O�����R���|�[�l���g�̊�b�N���X
    class Renderer : public Component 
    {
    public:
        void SetEnable(bool value);
        [[nodiscard]] bool GetEnabled() const { return enabled_; }

    protected:
        bool isCalled_ = false;     //���Ƀ}�l�[�W���[�ɓo�^����Ă��邩
        bool enabled_ = true;       //�`�悷�邩
        bool enabledOld_ = false;   //�ύX���g���K�[�p�L���b�V��
        bool canRender_ = false;    //�����_�����O�\���i�f�[�^�������Ă��Ȃ���)


        //int subsetCount_ = 0; //�T�u�Z�b�g�̐�
    private:
        virtual void Render(const int subsetNumber = 0) {}          //�`����s
        virtual void RenderShadow(const int subsetNumber = 0) {}    //�V���h�E�}�b�v�`����s
        virtual void RecalculateFrame() {};                         //���_�f�[�^���Čv�Z
    };
}


