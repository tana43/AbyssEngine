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

        ImGui::DragFloat3("Offset Potation", &offsetPos_.x,0.05f);
        ImGui::DragFloat3("Offset Rotation", &offsetRot_.x);
        ImGui::DragFloat3("Scale", &scale_.x,0.01f);

        ImGui::DragFloat3("World Position", &pos.X);
        ImGui::TreePop();
    }

    return false;
}

void ThrusterEffect::AttachSocket(std::string name)
{
    //�\�P�b�g�̃{�[���s���o�^����
     socketMatrix_ = attachModel_->FindSocket(name.c_str());
     socketName_ = name;
}

void ThrusterEffect::UpdateTransform()
{
    const auto& manager = EffectManager::Instance().GetEffekseerManager();
    if (!manager->Exists(effekseerHandle_))return;

    socketMatrix_ = attachModel_->FindSocket(socketName_.c_str());
    //Matrix socketMatrix = DirectX::XMLoadFloat4x4(socketGlobalTransform_);

    //�\�P�b�g�s�񂩂�e�����𒊏o
    Vector3 pos;
    Quaternion rot;
    Vector3 scale;
    socketMatrix_.Decompose(scale,rot,pos);

    //�s��X�V �I�t�Z�b�g�̉�]�l���l��
    auto q = Quaternion::Euler(offsetRot_);
    /*const Matrix S = Matrix::CreateScale(scale_);
    const Matrix R = Matrix::CreateFromQuaternion(rot);
    const Matrix T = Matrix::CreateTranslation(pos + offsetPos_);*/
    const Matrix S = Matrix::CreateScale(scale_);
    const Matrix OR = Matrix::CreateFromQuaternion(q);
    const Matrix OT = Matrix::CreateTranslation(offsetPos_);
    const Matrix OM = S * OR * OT;
    //socketMatrix_ = S * R * T;

    //���[���h�s��Z�o
    Matrix dxUe5 = DirectX::XMMatrixSet(-1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1); // LHS Y-Up Z-Forward(DX) -> LHS Z-Up Y-Forward(UE5) 
    //Matrix sm = dxUe5 * (S * R * T) * transform_->GetWorldMatrix();
    Matrix sm = dxUe5 * socketMatrix_ * OM * transform_->GetWorldMatrix();
    //sm = sm.Transform(*socketGlobalTransform_, q) * transform_->GetWorldMatrix();

    Effekseer::Matrix43 em = {
        sm._11,sm._12,sm._13,
        sm._21,sm._22,sm._23,
        sm._31,sm._22,sm._33,
        sm._41,sm._42,sm._43
    };
    //manager->SetMatrix(effekseerHandle_, mat);
    manager->SetMatrix(effekseerHandle_, em);
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

        //�G�t�F�N�g�Đ�����O�ɑO��̃G�t�F�N�g���~�����Ă���
        const auto& m = EffectManager::Instance().GetEffekseerManager();
        m->StopEffect(effekseerHandle_);
        effekseerHandle_ = effectEmitter_->Play();

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
