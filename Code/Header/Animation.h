#pragma once
#include <string>
#include "GeometricSubstance.h"

namespace AbyssEngine
{
    class SkeletalMesh;
    class GltfSkeletalMesh;
    class Animator;

    //�A�j���[�V�����f�[�^�̊��N���X
    //�V���v���ȍĐ��̏ꍇ���̃N���X�����̂܂܎g��
    class Animation
    {
    public:
        Animation(SkeletalMesh* model,const std::string& name_,const int& index,bool loop = true);
        ~Animation() {}

        //�A�j���[�V�����̍X�V
        virtual std::vector<GeometricSubstance::Node> UpdateAnimation(GltfSkeletalMesh* model,float& timeStamp);
        virtual void DrawImGui(Animator* animator);

    public:
        std::string name_;
        int animIndex_;//���f���{�̂������Ă��邱�̃��[�V�����̗v�f��
    protected: 

        bool loopFlag_;
        float animSpeed_ = 1.0f;//�e�A�j���[�V�������Ƃ̍Đ����x

        //���f���̎��m�[�h�փA�N�Z�X���邽�߂̃|�C���^
        std::vector<GeometricSubstance::Node> animatedNodes_;
    };


    //�u�����h�X�y�[�X1D
    //BlendWeight���P�݂̂̃��[�V�����u�����h
    class AnimBlendSpace1D final : public Animation
    {
    public:
        AnimBlendSpace1D(SkeletalMesh* model, const std::string& name_, const int& index0, const int& index1);
        AnimBlendSpace1D(SkeletalMesh* model, AnimBlendSpace1D animData);
        ~AnimBlendSpace1D() {}

        //�A�j���[�V�����̍X�V
        std::vector<GeometricSubstance::Node> UpdateAnimation(GltfSkeletalMesh* model, float& timeStamp)override;
        void DrawImGui(Animator* animator)override;

        //�u�����h����A�j���[�V�����̐��𑝂₷
        void AddBlendAnimation(
            const int& index,   /*���f���̒ǉ��A�j���[�V�����v�f�ԍ�*/
            const float& weight/*�ݒ肷��d��*/);

        void SetMaxWeight(const float& weight) { maxWeight_ = weight; }
        void SetMinWeight(const float& weight) { minWeight_ = weight; }
        void SetBlendWeight(const float& weight) { blendWeight_ = weight; }

        const float& GetMaxWeight() const { return maxWeight_; }
        const float& GetMinWeight() const { return minWeight_; }
        const float& GetBlendWeight() const { return blendWeight_; }

    private:
        float blendWeight_ = 0.0f;//�u�����h�̏d��
        float maxWeight_ = 1.0f;//�u�����h�̍ő�l
        float minWeight_ = 0.0f;//�u�����h�̍ŏ��l

        float lastBlendWeight_ = 0.0f;//�O��̃u�����h�̏d��

        //�u�����h����A�j���[�V����
        struct BlendAnimData
        {
            int index_;
            float weight_;//�X�̃��[�V�����������Ă���u�����h�̏d��
        };
        std::vector<BlendAnimData> blendAnimDatas_;

        std::vector<GeometricSubstance::Node> blendAnimNodes_[2];
    };

    //�u�����h�X�y�[�X2D
    //BlendWeight���Q���̃��[�V�����u�����h
    class AnimBlendSpace2D final : public Animation
    {
    public:
        enum class State
        {
            Idle,   //�ҋ@
            Move_F, //�O�i
            Move_R, //�E�ړ�
            Move_L, //���ړ�
            Move_B  //���
        };

        enum class BlendSituation//�ǂ̒��x�̃��[�V�������u�����h���K�v����\�����
        {
            None,//�K�v�Ȃ��A�ҋ@�̂܂܂ł悢
            Once,//�P��K�v�A�ҋ@�ƈړ��̃u�����h
            Twice,//�Q��K�v�A�ړ��ƈړ��̃u�����h���������ƁA����ɑҋ@�Ƃ��u�����h����
        };

        AnimBlendSpace2D(SkeletalMesh* model, const std::string& name_, const int& index, Vector2 weight = Vector2(0.0f,0.0f));
        ~AnimBlendSpace2D() {}

        //�A�j���[�V�����̍X�V
        std::vector<GeometricSubstance::Node> UpdateAnimation(GltfSkeletalMesh* model, float& timeStamp)override;
        void DrawImGui(Animator* animator)override;

        /// <summary>
        /// �ǉ����́A�ҋ@�A�E�A���A���̏��ԂɂȂ�悤�ɒ��ӁI�I�I
        /// �Ȃ��ҋ@�݂̂��̊֐����ĂԑO�ɐ������œ����Ă���͂�...
        /// </summary>
        void AddBlendAnimation(
            const int& index,   /*���f���̒ǉ��A�j���[�V�����v�f�ԍ�*/
            const Vector2& weight/*�ݒ肷��d��*/);

        void SetBlendWeight(const Vector2& weight) { blendWeight_ = weight; }


        struct BlendAnimData
        {
            int index_;
            Vector2 weight_;
        };
        std::vector<BlendAnimData>& GetBlendAnims() { return blendAnimDatas_; }
        void SetBlendAnims(std::vector<BlendAnimData>& anims) { blendAnimDatas_ = anims; }

    private:
        Vector2 maxWeight_ = { 1.0f,1.0f };//�u�����h�̍ő�l
        Vector2 minWeight_ = { -1.0f,-1.0f };//�u�����h�̍ŏ��l
        Vector2 blendWeight_ = { 0.0f,0.0f };//�u�����h�̏d��
        
        std::vector<BlendAnimData> blendAnimDatas_;

        Vector2 lastBlendWeight_ = {0,0};//�O��̃u�����h�̏d��

        std::vector<GeometricSubstance::Node> blendAnimNodes_[2];//�u�����h�Ɏg���m�[�h
        std::vector<GeometricSubstance::Node> secondBlendAnimNodes_;//����Ƀu�����h����̂Ɏg���m�[�h
    };
}



