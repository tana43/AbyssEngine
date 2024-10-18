#include "AttackerSystem.h"
#include "Engine.h"
#include "Character.h"
#include "GameCollider.h"
#include "Actor.h"
#include "imgui/imgui.h"

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
    durationTimer_ += actor_->GetDeltaTime();

    //�U�����L�����𔻒�
    AttackEnabledUpdate();
    
}

void AttackerSystem::DrawImGui()
{
    if (ImGui::TreeNode("AttackerSystem"))
    {
        for (auto& data : attackDataMap_)
        {
            if (ImGui::BeginMenu(data.first.c_str()))
            {
                auto& atk = data.second;
                ImGui::DragFloat("Power", &atk.power_, 0.1f);
                ImGui::DragFloat("Duration", &atk.duration_, 0.01f);
                ImGui::DragInt("Max Hit", &atk.maxHits_,1,0);
                ImGui::DragFloat("Hit Interval", &atk.hitInterval_, 0.01f);
                ImGui::DragFloat("Stagger Value", &atk.staggerValue_, 0.1f);
                ImGui::DragFloat("Hitstop Duration", &atk.hitStopDuration_, 0.1f,0.0f);
                ImGui::DragFloat("Hitstop OutTime", &atk.hitStopOutTime_, 0.1f,0.0f);
                ImGui::DragFloat("Knockback", &atk.knockback_, 0.1f,0.0f);

                ImGui::EndMenu();
            }
        }

        ImGui::TreePop();
    }
}

void AttackerSystem::OnCollision(const std::shared_ptr<Collider>& collision, Collision::IntersectionResult result)
{
    //��x�U�����q�b�g�����t���[����
    if (!attackEnabled_)return;

    //�_���[�W��^����
    const std::shared_ptr<Character>& target = collision->GetActor()->GetParent().lock()->GetComponent<Character>();
    if (target)
    {
        ApplyDamage(target);
    }

    //�U�����L�������X�V
    AttackEnabledUpdate();
}

void AbyssEngine::AttackerSystem::RegistAttackData(std::string atkName, const AttackData atkData)
{
    attackDataMap_.emplace(atkName, atkData);
}

void AbyssEngine::AttackerSystem::RegistCollider(const std::shared_ptr<AttackCollider>& collider)
{
    attackColliderList_.emplace_back(collider);
    collider->SetEnable(false);
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
    currentAttack_= atkData->second;

    //�e�l�����Z�b�g
    hitCount_ = 0;
    hitIntervalTimer_ = 1000.0f;//�ŏ��ɍU����������Ȃ����Ƃ��Ȃ��悤�ɑ傫�߂̒l���Z�b�g
    durationTimer_ = 0;

    //�w��R���C�_�[�̂݃A�N�e�B�u��
    for (const auto& p : attackColliderList_)
    {
        if (const auto& atk = p.lock())
        {
            atk->SetEnable(false);
            for (const auto& c : currentAttack_.attackColliderList_)
            {
                if (const auto& active = c.lock())
                {
                    if (atk == active)
                    {
                        atk->SetEnable(true);
                        break;
                    }
                }
            }
        }
    }
}

void AbyssEngine::AttackerSystem::ApplyDamage(const std::shared_ptr<Character>& target)
{
    Character::DamageResult result;
    Character::AttackParameter param;
    param.power_ = currentAttack_.power_;
    param.knockback_ = currentAttack_.knockback_;

    //�U���������Z�o
    Vector3 vec = target->GetTransform()->GetPosition() - transform_->GetPosition();
    vec.y *= 0.5f;//�㉺�ɂ͐�����΂���ɂ���
    vec.Normalize(param.vector_);

    if (target->ApplyDamage(param, &result))
    {
        //�U�����q�b�g����
        hit_ = true;
        hitCount_++;

        //�q�b�g�C���^�[�o�������Z�b�g
        hitIntervalTimer_ = 0;

        //���݂��Ƀq�b�g�X�g�b�v
        target->HitStop(currentAttack_.hitStopDuration_,currentAttack_.hitStopOutTime_);
        actor_->GetComponent<Character>()->HitStop(currentAttack_.hitStopDuration_, currentAttack_.hitStopOutTime_);
    }
}

void AbyssEngine::AttackerSystem::AttackEnabledUpdate()
{
    //�X�V�O�̒l
    bool cachedEnabled = attackEnabled_;

    //�U�����L���ȏ�Ԃ�����
    if (hit_ || /*��x�U�����q�b�g�����t���[����*/
        durationTimer_ > currentAttack_.duration_ ||/*�������Ԃ��z���Ă��Ȃ���*/
        hitCount_ >= currentAttack_.maxHits_ || /*�A���U���̍ő�q�b�g�����z���Ă��Ȃ���*/
        hitIntervalTimer_ < currentAttack_.hitInterval_)/*�q�b�g��̃C���^�[�o������*/
    {
        attackEnabled_ = false;
    }
    else
    {
        attackEnabled_ = true;
    }

    //�I���I�t���؂�ւ�����Ȃ�R���C�_�[���ݒ肵�Ȃ���
    if (attackEnabled_ != cachedEnabled)
    {
        for (const auto& col : currentAttack_.attackColliderList_)
        {
            if (const auto& collider = col.lock())
            {
                collider->SetEnable(attackEnabled_);
            }
        }
    }
}

AttackData& AbyssEngine::AttackData::operator=(const AttackData& data)
{
    power_              = data.power_;
    knockback_          = data.knockback_;
    duration_           = data.duration_;
    staggerValue_       = data.staggerValue_;
    maxHits_            = data.maxHits_;
    hitInterval_        = data.hitInterval_;
    hitStopDuration_    = data.hitStopDuration_;
    hitStopOutTime_     = data.hitStopOutTime_;
    attackColliderList_ = data.attackColliderList_;
    staggerType_        = data.staggerType_;
    return *this;
}
