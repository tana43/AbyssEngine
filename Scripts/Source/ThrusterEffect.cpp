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
    //ソケットのボーン行列を登録する
     socketGlobalTransform_ = &attachModel_->FindSocket(name.c_str());
}

void ThrusterEffect::UpdateTransform()
{
    const auto& manager = EffectManager::Instance().GetEffekseerManager();

    //行列更新 オフセットの回転値も考慮
    auto q = Quaternion::Euler(rotation_);
    socketGlobalTransform_->Transform(*socketGlobalTransform_,q);
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
