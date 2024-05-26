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

        void AnimatorUpdate();

        void PlayAnimation(size_t animIndex, bool loop = true);

        //�A�j���[�V�����f�[�^�̍ēǂݍ���
        void ReloadAnimation();

        //�A�j���[�V�����A�Z�b�g�̒ǉ�
        Animation& AppendAnimation(const std::string& filename,const std::string& motionName);
        void AppendAnimations(const std::vector<std::string>& filenames,const std::vector<std::string>& motionNames);

    private:
        //���ׂẴA�j���[�V����
        std::vector<Animation> animations_;

        std::weak_ptr<SkeletalMesh> skeletalMesh_;

        float timeStamp_ = 0.0f;//�^�C�}�[
        float animationSpeed_ = 1.0f;//�A�j���[�V�����Đ����x

        size_t animationClip_ = 0;//�Đ����̃A�j���[�V�����v�f�ԍ�
        bool animationLoop_ = true;//���[�v

        std::vector<GeometricSubstance::Node> animatedNodes_;
    };

}

