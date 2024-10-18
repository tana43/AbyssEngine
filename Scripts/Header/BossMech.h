#pragma once
#include "HumanoidWeapon.h"
#include <vector>

namespace AbyssEngine
{
    class SkeletalMesh;
    template <class T>
    class BehaviorTree;
    class AttackerSystem;
    class AttackCollider;
}

class Vitesse;

class BossMech : public HumanoidWeapon
{
public:
    enum class AnimationIndex
    {
        Idle,
        Run,
        Jump_Start,
        Jump_Loop,
        Jump_End,
        Crouching,//�N���E�`���O
    };


    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

    //�ːi
    void RushAttackUpdate();

public:
    const std::weak_ptr<Vitesse>& GetTargetVitesse() const { return targetVitesse_; }

private:
    //�R���C�_�[�ݒ�
    void ColliderInitialize();

    //�r�w�C�r�A�c���[������
    void BehaviorTreeInitialize();

    //�A�^�b�J�[�V�X�e��������
    void AttackerSystemInitialize();

public:
    const std::shared_ptr<AbyssEngine::AttackerSystem>& GetAttackerSystem() const { return attackerSystem_; }

private:
    //std::shared_ptr<AbyssEngine::SkeletalMesh> model_;

    //�^�[�Q�b�g
    std::weak_ptr<Vitesse> targetVitesse_;

    //�r�w�C�r�A�c���[
    std::shared_ptr<AbyssEngine::BehaviorTree<BossMech>> aiTree_;

    //�A�^�b�J�[�V�X�e��
    std::shared_ptr<AbyssEngine::AttackerSystem> attackerSystem_;

    //�U������p�R���C�_�[
    std::vector<std::shared_ptr<AbyssEngine::AttackCollider>> attackColliders_;

    //�ːi�U���̗U���̋����@
    float rushHoming_ = 1.0f;
};

