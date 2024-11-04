#pragma once
#include "ScriptComponent.h"
#include "MathHelper.h"
#include "Collider.h"

namespace AbyssEngine
{
    class Projectile;
    class BillboardRenderer;
    class ComputeParticleEmitter;
}

class Gun : public AbyssEngine::ScriptComponent
{
public:
    //発射する弾の種類
    enum class BulletType
    {
        Bullet,
        Beam,
        Max,
    };
   

    Gun() {}
    ~Gun() = default;

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    void DrawImGui()override;
    void DrawDebug()override;

    void Update()override;

    /// <summary>
    /// 射撃
    /// </summary>
    /// <param name="hootingDirection">弾を打つ方向(正規化させること)</param>
    bool Shot(AbyssEngine::Vector3 shootingDirection);

    //マズルフラッシュのエフェクトの座標更新
    void UpdateFlashEffect();
    void UpdateFlashParticleEffect();

public:
    const AbyssEngine::Vector3& GetMuzzlePos() const { return muzzlePos_; }
    void SetMuzzlePos(const AbyssEngine::Vector3& pos) { muzzlePos_ = pos; }

    const bool& GetADS() const { return ADS_; }
    void SetADS(const bool& flag) { ADS_ = flag; }

    const float& GetPrecision() const { return precision_; }
    void SetPrecision(const float& precision) { precision_ = precision; }

    void SetRateOfFire(const float& rate) { rateOfFire_ = rate; }

    const std::shared_ptr<AbyssEngine::BillboardRenderer>& GetMuzzleFlashComponent() const { return muzzleFlashComponent_; }

    void SetColliderTag(AbyssEngine::Collider::Tag tag) { colliderTag_ = static_cast<unsigned int>(tag); }

    void SetBulletType(BulletType type) { bulletType_ = type; }

    void SetBulletSpeed(const float& speed) { bulletSpeed_ = speed; }

    void SetBeamColor(const AbyssEngine::Vector4& color) { beamColor_ = color; }

    void SetBeamScale(const float& scale) { beamScale_ = scale; }

    void SetBeamWidth(const float& width) { beamWidth_ = width; }

    void SetHomingStrength(const float& str) { homingStrength_ = str; }

    void SetIsHoming(const bool& active) { isHoming_ = active; }

    void SetTargetTag(const unsigned int& tag) { targetTag_ = tag; }

private:
    //ADS(スコープを覗いているか)
    bool ADS_ = false;

    //銃口（弾の生成位置）
    AbyssEngine::Vector3 muzzlePos_;

    //発射感覚
    float rateOfFire_ = 0.05f;
    
    //発射間隔を計測するタイマー
    float rateTimer_;

    //マガジン容量
    int magazineSize_ = 30;

    //マガジン内の弾数
    int ammoInMag_ = magazineSize_ = 30;

    //どれだけ真ん中に球を撃つかの精度
    //float precision_ = 0.03f;
    float precision_ = 0.02f;

    //ADS時の精度
    float adsPrecision = 0.01f;

    //マズルフラッシュ用の画像
    std::shared_ptr<AbyssEngine::BillboardRenderer> muzzleFlashComponent_;
    std::shared_ptr<AbyssEngine::BillboardRenderer> beamMuzzleFlashComponent_;
    
    //エフェクト寿命
    float flashLifespan_ = 0.0f;
    float Max_Flash_Lifespan = 0.02f;

    //弾丸の大きさ
    float bulletRadius_ = 0.1f;

    //弾丸の判定につけるタグ
    unsigned int colliderTag_ = 0;

    //発射する弾の種類
    BulletType bulletType_ = BulletType::Bullet;

    //ビームの幅
    float beamWidth_ = 1.0f;
    //ビームのカラー
    AbyssEngine::Vector4 beamColor_ = {1,1,1,1};
    //ビームのビルボードエフェクトの大きさ
    float beamScale_ = 1.0f;

    //発砲する弾丸の速度
    float bulletSpeed_ = 50.0f;

    //発砲時に出現させるパーティクルエフェクト
    std::shared_ptr<AbyssEngine::ComputeParticleEmitter> particleEmitter_;
    float particleAmplitudeSpeed_ = 70.0f;//パーティクルが周りに散らばる速度
    float particleSpeed_ = 200.0f;//攻撃方向へ向かう速度
    float flashParticleLifespan_ = 0.1f;
    //AbyssEngine::Vector3 muzzleFlashDirection_;//マズルフラッシュを発生させるベクトル

    bool isHoming_ = false;
    float homingStrength_ = 1.0f;


    //目標に設定するアクターのタグ
    unsigned int targetTag_;
};

