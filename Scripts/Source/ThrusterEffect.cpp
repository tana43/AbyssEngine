#include "ThrusterEffect.h"
#include "Actor.h"
#include "EffectManager.h"
#include "SkeletalMesh.h"
#include "Engine.h"

#include "AssetManager.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;

int ThrusterEffect::instanceCount_ = 0;

ThrusterEffect::ThrusterEffect()
{
    name_ += std::to_string(instanceCount_);
    instanceCount_++;
}

void ThrusterEffect::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //������
    actor_ = actor;
    transform_ = actor->GetTransform();
    attachModel_ = actor->GetComponent<SkeletalMesh>();

    //�G�t�F�N�g�ǂݍ���
    //const std::string path = "./Assets/Effects/Thruster_01.efk";
    const std::string path = "./Assets/Effects/Thruster_02.efk";
    const auto it = Engine::assetManager_->cacheEffect_.find(path.c_str());
    if (it != Engine::assetManager_->cacheEffect_.end())
    {
        effectEmitter_ = it->second;
    }
    else
    {
        //��x���ǂݍ��܂�Ă��Ȃ��G�t�F�N�g�Ȃ�V���ɓǂݍ��݁A�A�Z�b�g�}�l�[�W���[�ɓo�^
        effectEmitter_ = std::make_shared<Effect>(path.c_str());
        Engine::assetManager_->cacheEffect_[path.c_str()] = effectEmitter_;
    }

    //Json�t�@�C���ǂݍ��݁A�����o��
    {
        //�t�@�C���̓ǂݍ���
        nlohmann::json mJson = actor_->ReadAndCreateJsonFile();
        if (mJson.find(name_.c_str()) != mJson.end())
        {
            //�ǂݍ��񂾃f�[�^�����ꂼ��̕ϐ��ɑ������
            auto& data = mJson[name_.c_str()];
            offsetPos_ = { data["OffsetPosition"][0],data["OffsetPosition"][1],data["OffsetPosition"][2] };
            offsetRot_ = { data["OffsetRotation"][0], data["OffsetRotation"][1], data["OffsetRotation"][2] };
            scale_ = { data["Scale"][0], data["Scale"][1], data["Scale"][2] };
        }
        else
        {
            //�f�[�^��������Ȃ������̂ō쐬
            mJson[name_.c_str()] = {
                {"OffsetPosition",{0.0f,0.0f,0.0f}},
                {"OffsetRotation",{0.0f,0.0f,0.0f}},
                {"Scale",{1.0f,1.0f,1.0f}},
            };

            //�t�@�C���ɓ��e����������
            actor_->WritingJsonFile(mJson);
        }
    }
}

void ThrusterEffect::DrawImGui()
{
    ImGui::SetNextItemOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);
    if (ImGui::TreeNode(name_.c_str()))
    {
        if (ImGui::Button("Fire"))
        {
            Fire();
        }

        if (ImGui::Button("Stop"))
        {
            Stop();
        }

        ImGui::SliderFloat("Power", &power_, 0.0f, 1.5f);
        ImGui::SliderFloat("Normal Power", &normalPower_, 0.0f, 1.5f);

        ImGui::DragFloat3("Offset Potation", &offsetPos_.x,0.01f);
        ImGui::DragFloat3("Offset Rotation", &offsetRot_.x);
        ImGui::DragFloat3("Scale", &scale_.x,0.01f);

        //Json�֕ۑ�
        if (ImGui::ButtonDoubleChecking("Save", doubleCheckFlag_))
        {
            SaveToJson();
        }

        if (ImGui::TreeNode("World Transform"))
        {
            const auto& m = EffectManager::Instance().GetEffekseerManager();
            //auto pos = m->GetLocation(effekseerHandle_);
            Effekseer::Matrix44 em;
            auto m43 = m->GetMatrix(effekseerHandle_);
            m43.ToMatrix44(em);
            Matrix mat = {
                em.Values[1][1],em.Values[1][2],em.Values[1][3],em.Values[1][4],
                em.Values[2][1],em.Values[2][2],em.Values[2][3],em.Values[2][4],
                em.Values[3][1],em.Values[3][2],em.Values[3][3],em.Values[3][4],
                em.Values[4][1],em.Values[4][2],em.Values[4][3],em.Values[4][4]
            };

            Vector3 pos, sca;
            Quaternion rot;
            mat.Decompose(sca, rot, pos);
            auto r = rot.To_Euler();
            //auto sca = m->GetScale(effekseerHandle_);
            ImGui::DragFloat3("World Position", &pos.x);
            ImGui::DragFloat3("World Rotation", &r.x);
            ImGui::DragFloat3("World Scale", &sca.x);

            ImGui::TreePop();

        }
        ImGui::TreePop();
    }
}

void ThrusterEffect::AttachSocket(const std::string& name)
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

    //�\�P�b�g�s�񂩂�e�����𒊏o
    Vector3 pos;
    Quaternion rot;
    Vector3 scale;
    socketMatrix_.Decompose(scale,rot,pos);

    //�s��X�V �I�t�Z�b�g�̉�]�l���l��
    auto q = Quaternion::Euler(offsetRot_);

    scale = scale_ * power_;
    const Matrix S = Matrix::CreateScale(scale_ * power_);
    const Matrix OR = Matrix::CreateFromQuaternion(q);
    const Matrix OT = Matrix::CreateTranslation(offsetPos_);
    const Matrix OM = S * OR * OT;
    //socketMatrix_ = S * R * T;

    //���[���h�s��Z�o
    Matrix dxUe5 = DirectX::XMMatrixSet(-1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1); // LHS Y-Up Z-Forward(DX) -> LHS Z-Up Y-Forward(UE5) 
    //Matrix sm = dxUe5 * (S * R * T) * transform_->GetWorldMatrix();
    Matrix sm = OM * dxUe5 * socketMatrix_ * transform_->GetWorldMatrix();
    //sm = sm.Transform(*socketGlobalTransform_, q) * transform_->GetWorldMatrix();

    Effekseer::Matrix43 em = {
        sm._11,sm._12,sm._13,
        sm._21,sm._22,sm._23,
        sm._31,sm._32,sm._33,
        sm._41,sm._42,sm._43
    };
    //manager->SetMatrix(effekseerHandle_, mat);
    manager->SetMatrix(effekseerHandle_, em);

    manager->SetAllColor(effekseerHandle_, Effekseer::Color(1000, 1000, 1000, 1000));
}

void ThrusterEffect::UpdateInjection()
{
    switch (sequence_)
    {
    case ThrusterEffect::Sequence::Standby://�X�^���o�C
        break;
    case ThrusterEffect::Sequence::Ignition://�_��

        power_ += ignitionSpeed_ * Time::deltaTime_;

        if (power_ >= 1.5f)
        {
            power_ = 1.5f;

            //���ˏ�Ԃֈڍs
            sequence_ = Sequence::Burning;
        }

        break;
    case ThrusterEffect::Sequence::Burning://���˒�
    {
        //Stop�֐����Ă΂��܂ōĐ���������

        //���ˏo�͂𐧌�
        const float ctrlPower = 5.0f * Time::deltaTime_;
        power_ += power_ > normalPower_ ? -ctrlPower : ctrlPower;

        break;
    }
    case ThrusterEffect::Sequence::Extinguishing://����

        power_ -= extSpeed_ * Time::deltaTime_;

        if (power_ <= 0.0f)
        {
            //�G�t�F�N�g���~
            effectEmitter_->Stop(effekseerHandle_);

            //�ҋ@��Ԃ֑J��
            power_ = 0.0f;
            sequence_ = Sequence::Standby;
        }

        break;

    case ThrusterEffect::Sequence::Boost:

        //0.1�b�Ō��̏o�͂ɖ߂�
        power_ -= 10.0f * boostPower_ *  Time::deltaTime_;

        //�o�͂����ʂ̏�Ԃɖ߂����畬�ˏ�Ԃ֑J��
        if (power_ < normalPower_)
        {
            sequence_ = Sequence::Burning;
        }

        break;

    default:
        break;
    }
}

void ThrusterEffect::Fire(const float normalPower)
{
    normalPower_ = normalPower;

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

void ThrusterEffect::SaveToJson()
{
    //�t�@�C���̓ǂݍ���
    nlohmann::json mJson = actor_->ReadAndCreateJsonFile();

    auto& data = mJson[name_.c_str()];
    data["OffsetPosition"] = { offsetPos_.x,offsetPos_.y,offsetPos_.z };
    data["OffsetRotation"] = { offsetRot_.x,offsetRot_.y,offsetRot_.z };
    data["Scale"] = { scale_.x,scale_.y,scale_.z };
    
    //�t�@�C���ɓ��e����������
    actor_->WritingJsonFile(mJson);
}

void ThrusterEffect::Boost(const float power)
{
    //�G�t�F�N�g���Đ�����Ă��Ȃ���΍Đ�����
    const auto& m = EffectManager::Instance().GetEffekseerManager();
    m->Exists(effekseerHandle_);

    //�o�͐ݒ�
    power_ = power;
    boostPower_ = power;

    //�V�[�P���X���u�[�X�g��
    sequence_ = Sequence::Boost;
}
