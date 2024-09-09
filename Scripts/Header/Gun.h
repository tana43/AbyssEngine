#pragma once
#include "ScriptComponent.h"
#include "MathHelper.h"

namespace AbyssEngine
{
    class Projectile;
    class BillboardRenderer;
}

class Gun : public AbyssEngine::ScriptComponent
{
public:
    Gun() {}
    ~Gun() = default;

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    bool DrawImGui()override;
    void DrawDebug()override;

    void Update()override;

    /// <summary>
    /// 射撃
    /// </summary>
    /// <param name="hootingDirection">弾を打つ方向(正規化させること)</param>
    bool Shot(AbyssEngine::Vector3 shootingDirection);

    //マズルフラッシュのエフェクトの座標更新
    void UpdateFlashEffect();

public:
    const AbyssEngine::Vector3& GetMuzzlePos() const { return muzzlePos_; }
    void SetMuzzlePos(const AbyssEngine::Vector3& pos) { muzzlePos_ = pos; }

    const bool& GetADS() const { return ADS_; }
    void SetADS(const bool& flag) { ADS_ = flag; }

    const float& GetPrecision() const { return precision_; }
    void SetPrecision(const float& precision) { precision_ = precision; }

    void SetRateOfFire(const float& rate) { rateOfFire_ = rate; }

    const std::shared_ptr<AbyssEngine::BillboardRenderer>& GetMuzzleFlashComponent() const { return muzzleFlashComponent_; }

    void SetColliderTag(const Collider::Tag& tag) { colliderTag_ = static_cast<unsigned int>(tag); }

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
    
    //エフェクト寿命
    float flashLifespan_ = 0.0f;
    float Max_Flash_Lifespan = 0.02f;

    //弾丸の大きさ
    float bulletRadius_;

    //弾丸の判定につけるタグ
    unsigned int colliderTag_;
};

