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
    //const std::string path = "./Assets/Effects/Thruster_01.efk";
    const std::string path = "./Assets/Effects/Thruster_02.efk";
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

    //Jsonファイル読み込み、書き出し
    {
        //ファイルの読み込み
        nlohmann::json mJson = actor_->ReadAndCreateJsonFile();
        if (mJson.find(name_.c_str()) != mJson.end())
        {
            //読み込んだデータをそれぞれの変数に代入する
            auto& data = mJson[name_.c_str()];
            offsetPos_ = { data["OffsetPosition"][0],data["OffsetPosition"][1],data["OffsetPosition"][2] };
            offsetRot_ = { data["OffsetRotation"][0], data["OffsetRotation"][1], data["OffsetRotation"][2] };
            scale_ = { data["Scale"][0], data["Scale"][1], data["Scale"][2] };
        }
        else
        {
            //データが見つからなかったので作成
            mJson[name_.c_str()] = {
                {"OffsetPosition",{0.0f,0.0f,0.0f}},
                {"OffsetRotation",{0.0f,0.0f,0.0f}},
                {"Scale",{1.0f,1.0f,1.0f}},
            };

            //ファイルに内容を書き込む
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

        //Jsonへ保存
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
    //ソケットのボーン行列を登録する
     socketMatrix_ = attachModel_->FindSocket(name.c_str());
     socketName_ = name;
}

void ThrusterEffect::UpdateTransform()
{
    const auto& manager = EffectManager::Instance().GetEffekseerManager();
    if (!manager->Exists(effekseerHandle_))return;

    socketMatrix_ = attachModel_->FindSocket(socketName_.c_str());

    //ソケット行列から各成分を抽出
    Vector3 pos;
    Quaternion rot;
    Vector3 scale;
    socketMatrix_.Decompose(scale,rot,pos);

    //行列更新 オフセットの回転値も考慮
    auto q = Quaternion::Euler(offsetRot_);

    scale = scale_ * power_;
    const Matrix S = Matrix::CreateScale(scale_ * power_);
    const Matrix OR = Matrix::CreateFromQuaternion(q);
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
    case ThrusterEffect::Sequence::Standby://スタンバイ
        break;
    case ThrusterEffect::Sequence::Ignition://点火

        power_ += ignitionSpeed_ * Time::deltaTime_;

        if (power_ >= 1.5f)
        {
            power_ = 1.5f;

            //噴射状態へ移行
            sequence_ = Sequence::Burning;
        }

        break;
    case ThrusterEffect::Sequence::Burning://噴射中
    {
        //Stop関数が呼ばれるまで再生し続ける

        //噴射出力を制御
        const float ctrlPower = 5.0f * Time::deltaTime_;
        power_ += power_ > normalPower_ ? -ctrlPower : ctrlPower;

        break;
    }
    case ThrusterEffect::Sequence::Extinguishing://鎮火

        power_ -= extSpeed_ * Time::deltaTime_;

        if (power_ <= 0.0f)
        {
            //エフェクトを停止
            effectEmitter_->Stop(effekseerHandle_);

            //待機状態へ遷移
            power_ = 0.0f;
            sequence_ = Sequence::Standby;
        }

        break;

    case ThrusterEffect::Sequence::Boost:

        //0.1秒で元の出力に戻す
        power_ -= 10.0f * boostPower_ *  Time::deltaTime_;

        //出力が普通の状態に戻ったら噴射状態へ遷移
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

void ThrusterEffect::SaveToJson()
{
    //ファイルの読み込み
    nlohmann::json mJson = actor_->ReadAndCreateJsonFile();

    auto& data = mJson[name_.c_str()];
    data["OffsetPosition"] = { offsetPos_.x,offsetPos_.y,offsetPos_.z };
    data["OffsetRotation"] = { offsetRot_.x,offsetRot_.y,offsetRot_.z };
    data["Scale"] = { scale_.x,scale_.y,scale_.z };
    
    //ファイルに内容を書き込む
    actor_->WritingJsonFile(mJson);
}

void ThrusterEffect::Boost(const float power)
{
    //エフェクトが再生されていなければ再生する
    const auto& m = EffectManager::Instance().GetEffekseerManager();
    m->Exists(effekseerHandle_);

    //出力設定
    power_ = power;
    boostPower_ = power;

    //シーケンスをブーストに
    sequence_ = Sequence::Boost;
}
