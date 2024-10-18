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
        void DrawImGui()override;

        //SkeletalMesh����Ă΂�鏉�����@�ʏ�̏������ł͂ł��Ȃ�����
        void LatterInitialize(const std::shared_ptr<SkeletalMesh>& skeletalMesh);

        void AnimatorUpdate();

        //�A�j���[�V�����Đ��@���[�V�����̑J�ڎ��Ԃ��w��@���������ꍇ�̓����o�ϐ��̒l����������
        void PlayAnimation(const size_t& animIndex,float* transTime = nullptr,float startTime = 0.0f);//�v�f�����猟��
        void PlayAnimation(const std::string& animName,float* transTime = nullptr,float startTime = 0.0f);//���O���猟��
    private:
        void PlayAnimationCommon(const size_t& animIndex,float transTime,float startTime = 0.0f);

    public:
        //�A�j���[�V�����f�[�^�̍ēǂݍ���
        //void ReloadAnimation();

        //�A�j���[�V�����A�Z�b�g�̒ǉ�
        void AppendAnimation(const std::string& filename,const std::string& motionName);
        void AppendAnimations(const std::vector<std::string>& filenames,const std::vector<std::string>& motionNames);
        AnimBlendSpace1D* AppendAnimation(AnimBlendSpace1D anim);
        AnimBlendSpace2D* AppendAnimation(AnimBlendSpace2D anim);
        AnimBlendSpaceFlyMove* AppendAnimation(AnimBlendSpaceFlyMove anim);

        std::vector<GeometricSubstance::Node>& GetAnimatedNodes() { return animatedNodes_; }
        void SetAnimatedNodes(const std::vector<GeometricSubstance::Node>& nodes) { animatedNodes_ = nodes; }

        void SetSkeletalMesh(const std::shared_ptr<SkeletalMesh>& mesh) { skeletalMesh_ = mesh; }

        //���݂̃A�j���[�V�������擾
        const int GetCurrentAnimClip() const { return static_cast<int>(animationClip_); }
        //���̃A�j���[�V�������擾
        const int GetNextAnimClip() const { return static_cast<int>(nextAnimationClip_); }

        std::vector<Animation*> GetAnimations();

        const bool& GetAnimationFinished() const { return isFinished_; }
        
        //���[�g�W���C���g�̃C���f�b�N�X��ݒ�
        //�����[�g���[�V�����ɕK�{
        void SetRootJointIndex(const int& index) { rootJointIndex_ = index; }

        void SetEnableRootMotion(const bool& enable) { enableRootMotion_ = enable; }

        const Vector3& GetRootMotionMove() const { return rootMotionMove_; }
        void SetRootMotionMove(const Vector3& move) { rootMotionMove_ = move; }

        void SetAnimationTransTime(const float& time) { animationTransTime_ = time; }
    private:
        //���ׂẴA�j���[�V����
        std::vector<std::unique_ptr<Animation>> animations_;

        std::weak_ptr<SkeletalMesh> skeletalMesh_;

        float timeStamp_ = 0.0f;//�^�C�}�[
        float animationSpeed_ = 1.0f;//�A�j���[�V�����Đ����x

        float animationTransTime_ = 0.1f;//�A�j���[�V�����̑J�ڎ���

        size_t animationClip_ = 0;//�Đ����̃A�j���[�V�����ԍ�
        size_t nextAnimationClip_ = -1;//���ɍĐ��A�j���[�V�����ԍ�
        //bool animationLoop_ = true;//���[�v

        std::vector<GeometricSubstance::Node> animatedNodes_;//���݂̃A�j���[�V����
        std::vector<GeometricSubstance::Node> nextAnimatedNodes_;//�J�ڐ�̃A�j���[�V����

        bool isTransitioningBlendAnim_ = false;//�A�j���[�V�����̑J�ڂ̃u�����h�����H
        float transitionTimer_ = 0;//�A�j���[�V�����J�ڃ^�C�}�[
        float transitionTimeRequired_ = 0;//�A�j���[�V�����̑J�ڂɊ|���鎞��

        bool isFinished_;//�A�j���[�V�����̍Đ����I����Ă��邩

        bool enableRootMotion_ = false;//���[�g���[�V����
        std::vector<GeometricSubstance::Node> zeroAnimatedNodes_;//�����p��
        bool isSetZeroAnimatedNodes_ = false;//�����p����ݒ肵�Ă��邩
        int rootJointIndex_ = 0;//���[�g�{�[���i�ł���̊K�w�ɂ���{�[���j�̃C���f�b�N�X
        Vector3 previousPosition_;//�O��̈ʒu

        //���[�g���[�V�����ɂ���Ĉړ�����l(�ʓr���Z�b�g���K�v)
        Vector3 rootMotionMove_;

    };

}

