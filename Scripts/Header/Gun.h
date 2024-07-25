#pragma once
#include "ScriptComponent.h"
#include "MathHelper.h"

namespace AbyssEngine
{
    class Projectile;
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

public:
    const AbyssEngine::Vector3& GetMuzzlePos() const { return muzzlePos_; }
    void SetMuzzlePos(const AbyssEngine::Vector3& pos) { muzzlePos_ = pos; }

    const bool& GetADS() const { return ADS_; }
    void SetADS(const bool& flag) { ADS_ = flag; }
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
    float precision_ = 0.03f;

    //ADS時の精度
    float adsPrecision = 0.01f;

};

