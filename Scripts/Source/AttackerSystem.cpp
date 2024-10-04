#include "AttackerSystem.h"
#include "Engine.h"
#include "Character.h"
#include "GameCollider.h"
#include "Actor.h"

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
    //��x�U�����q�b�g�����t���[����
    if (hit_)return;

    //�������Ԃ��z���Ă��Ȃ���
    if (durationTimer_ > currentAttack_.duration_)return;

    //�A���U��
    if (hitCount_ >= currentAttack_.maxHits_)return;

    //�q�b�g��̃C���^�[�o������
    if (hitIntervalTimer_ < currentAttack_.hitInterval_)return;


    //�_���[�W��^����
    const std::shared_ptr<Character>& target = collision->GetActor()->GetComponent<Character>();
    if (target)
    {
        ApplyDamage(target);
    }
}

void AbyssEngine::AttackerSystem::RegistAttackData(std::string atkName, const AttackData atkData)
{
    attackDataMap_.emplace(atkName, atkData);
}

void AbyssEngine::AttackerSystem::RegistCollider(const std::shared_ptr<AttackCollider>& collider)
{
    attackColliderList_.emplace_back(collider);
}

void AbyssEngine::AttackerSystem::Attack(std::string atkName)
{
    auto atkData = attackDataMap_.find(atkName);
    if (atkData == attackDataMap_.end())
    {
        _ASSERT_EXPR(false, L"�w��̖��O��AttackData�����݂��܂���");
        return;
    }
    
    //�U���̏���ݒ�
    currentAttack_.power_               = atkData->second.power_;
    currentAttack_.duration_            = atkData->second.duration_;
    currentAttack_.staggerValue_        = atkData->second.staggerValue_;
    currentAttack_.maxHits_             = atkData->second.maxHits_;
    currentAttack_.hitInterval_         = atkData->second.hitInterval_;
    currentAttack_.hitStop_             = atkData->second.hitStop_;
    currentAttack_.attackColliderList_  = atkData->second.attackColliderList_;

    //�e�l�����Z�b�g
    hitCount_ = 0;
    hitIntervalTimer_ = 1000.0f;//�ŏ��ɍU����������Ȃ����Ƃ��Ȃ��悤�ɑ傫�߂̒l���Z�b�g
    durationTimer_ = 0;

    //�w��R���C�_�[�̂݃A�N�e�B�u��
    for (const auto& p : attackColliderList_)
    {
        if (const auto& atk = p.lock())
        {
            for (const auto& c : currentAttack_.attackColliderList_)
            {
                if (const auto& active = c.lock())
                {
                    if (atk == active)
                    {
                        atk->SetEnable(true);
                    }
                    else
                    {
                        atk->SetEnable(false);
                    }
                }
            }
        }
    }
}

void AbyssEngine::AttackerSystem::ApplyDamage(const std::shared_ptr<Character>& target)
{
    Character::DamageResult result;
    if (target->ApplyDamage(currentAttack_.power_, result))
    {
        //�U�����q�b�g����
        hit_ = true;
        hitCount_++;

        //�q�b�g�C���^�[�o�������Z�b�g
        hitIntervalTimer_ = 0;
    }
}



