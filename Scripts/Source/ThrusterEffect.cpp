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
    //初期化
    actor_ = actor;
    transform_ = actor->GetTransform();
    attachModel_ = actor->GetComponent<SkeletalMesh>();

    //エフェクト読み込み
    const std::string path = "./Assets/Effects/Thruster_01.efk";
    const auto it = Engine::assetManager_->cacheEffect_.find(path.c_str());
    if (it != Engine::assetManager_->cacheEffect_.end())
    {
        effectEmitter_ = it->second;
    }
    else
    {
        //一度も読み込まれていないエフェクトなら新たに読み込み、アセットマネージャーに登録
        effectEmitter_ = std::make_shared<Effect>(path.c_str());
        Engine::assetManager_->cacheEffect_[path.c_str()] = effectEmitter_;
    }
}

bool ThrusterEffect::DrawImGui()
{
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

void ThrusterEffect::AttachSocket(const std::string& name)
{
    //ソケットのボーン行列を登録する
     socketMatrix_ = attachModel_->FindSocket(name.c_str());
     socketName_ = name;
}

void ThrusterEffect::UpdateTransform()
{
    const auto& manager = EffectManager::Instance().GetEffekseerManager();
    if (!manager->Exists(effekseerHandle_))return;

    socketMatrix_ = attachModel_->FindSocket(socketName_.c_str());
    //Matrix socketMatrix = DirectX::XMLoadFloat4x4(socketGlobalTransform_);

    //ソケット行列から各成分を抽出
    Vector3 pos;
    Quaternion rot;
    Vector3 scale;
    socketMatrix_.Decompose(scale,rot,pos);

    //行列更新 オフセットの回転値も考慮
    //auto q = Quaternion::Euler(offsetRot_);
    /*const Matrix S = Matrix::CreateScale(scale_);
    const Matrix R = Matrix::CreateFromQuaternion(rot);
    const Matrix T = Matrix::CreateTranslation(pos + offsetPos_);*/
    const Matrix S = Matrix::CreateScale(scale_ * power_);
    //const Matrix OR = Matrix::CreateFromQuaternion(q);
    const Vector3 r = { 
        DirectX::XMConvertToRadians(offsetRot_.x),
        DirectX::XMConvertToRadians(offsetRot_.y),
        DirectX::XMConvertToRadians(offsetRot_.z) };
    const Matrix OR = Matrix::CreateFromYawPitchRoll(r.y,r.x,r.z);
    const Matrix OT = Matrix::CreateTranslation(offsetPos_);
    const Matrix OM = S * OR * OT;
    //socketMatrix_ = S * R * T;

    //ワールド行列算出
    Matrix dxUe5 = DirectX::XMMatrixSet(-1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1); // LHS Y-Up Z-Forward(DX) -> LHS Z-Up Y-Forward(UE5) 
    //Matrix sm = dxUe5 * (S * R * T) * transform_->GetWorldMatrix();
    Matrix sm = OM * dxUe5 * socketMatrix_ * transform_->GetWorldMatrix();
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
    case ThrusterEffect::Sequence::Standby://スタンバイ
        break;
    case ThrusterEffect::Sequence::Ignition://点火

        power_ += ignitionSpeed_ * Time::deltaTime_;

        if (power_ >= 1)
        {
            power_ = 1.0f;

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

        //エフェクト再生する前に前回のエフェクトを停止させておく
        const auto& m = EffectManager::Instance().GetEffekseerManager();
        m->StopEffect(effekseerHandle_);
        effekseerHandle_ = effectEmitter_->Play();

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
