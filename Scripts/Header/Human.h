#pragma once
#include "Character.h"

namespace AbyssEngine
{
    class SkeletalMesh;
    class Animator;
    class AnimBlendSpace1D;
}

//�l�^�̃��f�����g���L�����N�^�[�Ɏg���R���|�[�l���g
class Human : public AbyssEngine::Character
{
public:
    virtual void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    virtual void Update()override {}
    //virtual void DelayedUpdate() {}//�`�掞�̍s��X�V�Ȃǂ��I����Ă���

    void DrawImGui()override;

public:
    const std::shared_ptr<AbyssEngine::SkeletalMesh>& GetModel() { return model_; }
    const std::shared_ptr<AbyssEngine::Animator>& GetAnimator();

protected:
    std::shared_ptr<AbyssEngine::SkeletalMesh> model_;

    AbyssEngine::AnimBlendSpace1D* moveAnimation_;//����ړ�
};

