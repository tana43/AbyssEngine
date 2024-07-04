#include "ThrusterEffect.h"
#include "Actor.h"
#include "EffectManager.h"
#include "SkeletalMesh.h"
#include "Engine.h"

using namespace AbyssEngine;

std::unique_ptr<Effect> ThrusterEffect::effectEmitter_;

ThrusterEffect::ThrusterEffect()
{
    if (effectEmitter_.get() == nullptr)
    {
        effectEmitter_ = std::make_unique<Effect>("./Assets/Effects/Thruster_01.efk");
    }
}

void ThrusterEffect::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();
    attachModel_ = GetComponent<SkeletalMesh>();
}

void ThrusterEffect::AttachSocket(std::string name)
{
    //�\�P�b�g�̃{�[���s���o�^����
     socketGlobalTransform_ = &attachModel_->FindSocket(name.c_str());
}

void ThrusterEffect::UpdateTransform()
{
    const auto& manager = EffectManager::Instance().GetEffekseerManager();

    //�s��X�V �I�t�Z�b�g�̉�]�l���l��
    auto q = Quaternion::Euler(rotation_);
    socketGlobalTransform_->Transform(*socketGlobalTransform_,q);
}

void ThrusterEffect::UpdateInjection()
{
    switch (sequence_)
    {
    case ThrusterEffect::Sequence::Standby://�X�^���o�C
        break;
    case ThrusterEffect::Sequence::Ignition://�_��
        power_ += ignitionSpeed_ * Time::deltaTime_;

        if (power_ >= 1)
        {
            //���ˏ�Ԃֈڍs
            sequence_ = Sequence::Burning;
        }

        break;
    case ThrusterEffect::Sequence::Burning://���˒�
        //Stop�֐����Ă΂��܂ōĐ���������


        break;
    case ThrusterEffect::Sequence::Extinguishing://����
        break;
    default:
        break;
    }
}

void ThrusterEffect::Fire()
{
    //���˂���Ă��Ȃ���Γ_�΂���
    if (sequence_ == Sequence::Standby || sequence_ == Sequence::Extinguishing)
    {
        sequence_ = Sequence::Ignition;

        //�o�͂��O��
        power_ = 0.0f;
    }
}

void ThrusterEffect::Stop()
{
    //���˂��Ă���̂Ȃ���΂���
    if (sequence_ == Sequence::Standby || sequence_ == Sequence::Burning)
    {
        sequence_ = Sequence::Extinguishing;

        //�o�͂��ő��
        power_ = 1.0f;
    }
}
