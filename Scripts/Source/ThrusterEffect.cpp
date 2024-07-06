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
    //ソケットのボーン行列を登録する
     socketGlobalTransform_ = &attachModel_->FindSocket(name.c_str());
}

void ThrusterEffect::UpdateTransform()
{
    const auto& manager = EffectManager::Instance().GetEffekseerManager();

    //行列更新 オフセットの回転値も考慮
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
    case ThrusterEffect::Sequence::Standby://スタンバイ
        break;
    case ThrusterEffect::Sequence::Ignition://点火

        power_ += ignitionSpeed_ * Time::deltaTime_;

        if (power_ >= 1)
        {
            //噴射状態へ移行
            sequence_ = Sequence::Burning;
        }

        break;
    case ThrusterEffect::Sequence::Burning://噴射中
        //Stop関数が呼ばれるまで再生し続ける


        break;
    case ThrusterEffect::Sequence::Extinguishing://鎮火

        power_ -= ExtSpeed_ * Time::deltaTime_;

        if (power_ <= 0.0f)
        {
            //エフェクトを停止
            effectEmitter_->Stop(effekseerHandle_);

            //待機状態へ遷移
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
    //噴射されていなければ点火する
    if (sequence_ == Sequence::Standby || sequence_ == Sequence::Extinguishing)
    {
        sequence_ = Sequence::Ignition;

        //出力を０に
        power_ = 0.0f;

        //エフェクト再生
        effekseerHandle_ = effectEmitter_->Play();

        const auto& m = EffectManager::Instance().GetEffekseerManager();
        UpdateTransform();
    }
}

void ThrusterEffect::Stop()
{
    //噴射しているのなら鎮火する
    if (sequence_ == Sequence::Standby || sequence_ == Sequence::Burning)
    {
        sequence_ = Sequence::Extinguishing;

        //出力を最大に
        power_ = 1.0f;
    }
}
