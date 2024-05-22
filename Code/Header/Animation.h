#pragma once
#include <string>

namespace AbyssEngine
{
    //�A�j���[�V�����f�[�^�̊��N���X
    //�V���v���ȍĐ��̏ꍇ���̃N���X�����̂܂܎g��
    class Animation
    {
    public:
        Animation(const std::string& name,const int& index,bool loop = true);
        ~Animation() {}

        //�A�j���[�V�����̍X�V
        virtual void UpdateAnimation();

    protected:
        std::string name_;
        int animIndex_;

        float animSpeed_ = 1.0f;//�e�A�j���[�V�������Ƃ̍Đ����x
        bool loopFlag_ = true;
    };

    //�u�����h�X�y�[�X1D
    //BlendWeight���P�݂̂̃��[�V�����u�����h
    class BlendSpace1D final : public Animation
    {
    public:
        BlendSpace1D(const std::string& name, const int& index,bool loop) : Animation(name, index,loop) {}
        ~BlendSpace1D() {}

    private:
        float blendWeight_ = 0.0f;
    };
}

