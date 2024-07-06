#include "ThrusterEffect.h"
#include "Actor.h"
#include "EffectManager.h"
#include "SkeletalMesh.h"
#include "Engine.h"

#include "imgui/imgui.h"

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
    attachModel_ = actor->GetComponent<SkeletalMesh>();
}

bool ThrusterEffect::DrawImGui()
{
    if (ImGui::TreeNode("ThrusterEffect"))
    {
        if (ImGui::Button("Fire"))
        {
            Fire();
        }

        if (ImGui::Button("Stop"))
        {
            Stop();
        }

        const auto& m = EffectManager::Instance().GetEffekseerManager();
        auto pos = m->GetLocation(effekseerHandle_);

        ImGui::DragFloat3("position", &pos.X);

        ImGui::TreePop();
    }

    return false;
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
    Matrix socketMatrix = DirectX::XMLoadFloat4x4(socketGlobalTransform_);
    const auto& sm = socketMatrix.Transform(*socketGlobalTransform_, q);
    Effekseer::Matrix43 mat = {
        sm._11,sm._12,sm._13,
        sm._21,sm._22,sm._23,
        sm._31,sm._22,sm._33,
        sm._41,sm._42,sm._43
    };
    //manager->SetMatrix(effekseerHandle_, mat);
    manager->SetBaseMatrix(effekseerHandle_, mat);
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

        power_ -= ExtSpeed_ * Time::deltaTime_;

        if (power_ <= 0.0f)
        {
            //�G�t�F�N�g���~
            effectEmitter_->Stop(effekseerHandle_);

            //�ҋ@��Ԃ֑J��
            power_ = 0.0f;
            sequence_ = Sequence::Standby;
        }

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

        //�G�t�F�N�g�Đ�
        effekseerHandle_ = effectEmitter_->Play();

        const auto& m = EffectManager::Instance().GetEffekseerManager();
        UpdateTransform();
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
