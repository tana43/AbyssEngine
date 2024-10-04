#include "AttackerSystem.h"
#include "Engine.h"

using namespace AbyssEngine;

void AttackerSystem::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //������
    ScriptComponent::Initialize(actor);
}

void AttackerSystem::Update()
{
    //�q�b�g�t���O���Z�b�g
    hit_ = false;

    //�^�C�}�[�X�V
    durationTimer_ += Time::deltaTime_;
}

void AbyssEngine::AttackerSystem::OnCollision(const std::shared_ptr<Collider>& collision, Collision::IntersectionResult result)
{
    //�������Ԃ��z���Ă��Ȃ���
    if (durationTimer_ > currentAttack_.duration_)return;
}

void AbyssEngine::AttackerSystem::RegistAttackData(std::string atkName, const AttackData atkData)
{
    attackDataMap_.emplace(atkName, atkData);
}



