#pragma once
#include <string>
#include "GeometricSubstance.h"

namespace AbyssEngine
{
    class SkeletalMesh;
    class GltfSkeletalMesh;

    //�A�j���[�V�����f�[�^�̊��N���X
    //�V���v���ȍĐ��̏ꍇ���̃N���X�����̂܂܎g��
    class Animation
    {
    public:
        Animation(SkeletalMesh* model,const std::string& name,const int& index,bool loop = false);
        ~Animation() {}

        //�A�j���[�V�����̍X�V
        virtual void UpdateAnimation(GltfSkeletalMesh* model,float& timeStamp);

    protected:
        std::string name_;
        int animIndex_;

        float animSpeed_ = 1.0f;//�e�A�j���[�V�������Ƃ̍Đ����x
        bool loopFlag_;

        //���f���̎��m�[�h�փA�N�Z�X���邽�߂̃|�C���^
        std::vector<GeometricSubstance::Node>* animatedNodes_;
    };

    //�u�����h�X�y�[�X1D
    //BlendWeight���P�݂̂̃��[�V�����u�����h
    class AnimBlendSpace1D final : public Animation
    {
    public:
        AnimBlendSpace1D(SkeletalMesh* model, const std::string& name, const int& index0, const int& index1);
        ~AnimBlendSpace1D() {}

        //�A�j���[�V�����̍X�V
        void UpdateAnimation(GltfSkeletalMesh* model, float& timeStamp);

        //�u�����h����A�j���[�V�����̐��𑝂₷
        void AddBlendAnimation(
            const int& index,   /*���f���̒ǉ��A�j���[�V�����v�f�ԍ�*/
            const float& weight/*�ݒ肷��d��*/);

        void SetMaxWeight(const float& weight) { maxWeight_ = weight; }
        void SetMinWeight(const float& weight) { minWeight_ = weight; }

    private:
        float blendWeight_ = 0.0f;//�u�����h�̏d��
        float maxWeight_ = 1.0f;//�u�����h�̍ő�l
        float minWeight_ = 0.0f;//�u�����h�̍ŏ��l

        //�u�����h����A�j���[�V����
        struct BlendAnim
        {
            int index_;
            float weight_;//�X�̃��[�V�����������Ă���u�����h�̏d��
        };
        std::vector<BlendAnim> blendAnims_;

        std::vector<GeometricSubstance::Node> blendAnimNodes_[2];
    };
}

