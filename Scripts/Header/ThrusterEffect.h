#pragma once
#include "Effect.h"
#include "MathHelper.h"
#include "Component.h"

namespace AbyssEngine
{
    class SkeletalMesh;
}

class ThrusterEffect : public AbyssEngine::Component
{
private:
    //噴射行程
    enum class Sequence
    {
        Standby,        //待機
        Ignition,       //点火
        Burning,        //噴射中
        Extinguishing,  //鎮火
    };

public:
    ThrusterEffect();
    ~ThrusterEffect() {}

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

    bool CanMultiple()override { return true; }

    bool DrawImGui()override;

public:

    //ソケットにアタッチする
    void AttachSocket(std::string name);

    //オフセット回転値を設定
    void SetRotation(const AbyssEngine::Vector3& rotation) { rotation_ = rotation; }
    void SetScale(const AbyssEngine::Vector3& scale) { scale_ = scale; }
    void SetScale(const float& scale) { scale_ = { scale,scale,scale }; }

    void UpdateTransform();

    //エフェクト更新（出力に合わせたスケール調整など）
    void UpdateInjection();

    //エフェクト再生開始
    void Fire();

    //エフェクト停止
    void Stop();

private:
    //再生しているエフェクトを管理している変数
    Effekseer::Handle effekseerHandle_;

    //エフェクト生成をするための変数　読み込んだエフェクトの情報を格納している
    static std::unique_ptr<Effect> effectEmitter_;

    std::shared_ptr<AbyssEngine::SkeletalMesh> attachModel_;
    std::string socketName_;
    DirectX::XMFLOAT4X4* socketGlobalTransform_;

    //オフセット値
    AbyssEngine::Vector3 rotation_;
    AbyssEngine::Vector3 scale_;

    //現在の噴射行程
    Sequence sequence_ = Sequence::Standby;

public:
    //出力(1~0の範囲、スラスター噴射の強弱)
    float power_ = 0.0f;

    //スラスター点火速度
    float ignitionSpeed_ = 5.0f;

    //スラスター鎮火速度
    float ExtSpeed_ = 5.0f;
};

