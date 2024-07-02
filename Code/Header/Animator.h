#pragma once
#include "Animation.h"
#include "Component.h"

namespace AbyssEngine
{
    class SkeletalMesh;

    class Animator final : public Component
    {
    public:
        Animator() {}
        ~Animator() {}

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        bool DrawImGui()override;

        //SkeletalMesh����Ă΂�鏉�����@�ʏ�̏������ł͂ł��Ȃ�����
        void LatterInitialize(const std::shared_ptr<SkeletalMesh>& skeletalMesh);

        void AnimatorUpdate();

        //�A�j���[�V�����Đ��@���[�V�����̑J�ڎ��Ԃ��w��
        void PlayAnimation(const size_t& animIndex,float transTime = 0.1f);//�v�f�����猟��
        void PlayAnimation(const std::string& animName,float transTime = 0.1f);//���O���猟��
    private:
        void PlayAnimationCommon(const size_t& animIndex,float transTime);

    public:
        //�A�j���[�V�����f�[�^�̍ēǂݍ���
        //void ReloadAnimation();

        //�A�j���[�V�����A�Z�b�g�̒ǉ�
        void AppendAnimation(const std::string& filename,const std::string& motionName);
        void AppendAnimations(const std::vector<std::string>& filenames,const std::vector<std::string>& motionNames);
        AnimBlendSpace1D* AppendAnimation(AnimBlendSpace1D anim);
        AnimBlendSpace2D* AppendAnimation(AnimBlendSpace2D anim);

        std::vector<GeometricSubstance::Node>& GetAnimatedNodes() { return animatedNodes_; }
        void SetAnimatedNodes(const std::vector<GeometricSubstance::Node>& nodes) { animatedNodes_ = nodes; }

        void SetSkeletalMesh(const std::shared_ptr<SkeletalMesh>& mesh) { skeletalMesh_ = mesh; }
    private:
        //���ׂẴA�j���[�V����
        std::vector<std::unique_ptr<Animation>> animations_;

        std::weak_ptr<SkeletalMesh> skeletalMesh_;

        float timeStamp_ = 0.0f;//�^�C�}�[
        float animationSpeed_ = 1.0f;//�A�j���[�V�����Đ����x

        size_t animationClip_ = 0;//�Đ����̃A�j���[�V�����ԍ�
        size_t nextAnimationClip_ = 0;//���ɍĐ��A�j���[�V�����ԍ�
        //bool animationLoop_ = true;//���[�v

        std::vector<GeometricSubstance::Node> animatedNodes_;//���݂̃A�j���[�V����
        std::vector<GeometricSubstance::Node> nextAnimatedNodes_;//�J�ڐ�̃A�j���[�V����

        bool isTransitioningBlendAnim_ = false;//�A�j���[�V�����̑J�ڂ̃u�����h�����H
        float transitionTimer_ = 0;//�A�j���[�V�����J�ڃ^�C�}�[
        float transitionTimeRequired_ = 0;//�A�j���[�V�����̑J�ڂɊ|���鎞��
    };

}

