#pragma once
#include "ScriptComponent.h"
#include "MathHelper.h"
#include "Collider.h"
#include "ComputeParticleEmitter.h"

namespace AbyssEngine
{
    class Projectile;
    class BillboardRenderer;

    class AudioSource;
}

class Gun : public AbyssEngine::ScriptComponent
{
public:
    //発射する弾の種類
    enum class BulletType
    {
        Bullet,
        Beam,
        Missile,
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
    bool Shot(AbyssEngine::Vector3 shootingDirection, AbyssEngine::Vector3* terrainHitPosition = nullptr);

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
    const std::shared_ptr<AbyssEngine::BillboardRenderer>& GetBeamMuzzleFlashComponent() const { return beamMuzzleFlashComponent_; }

    void SetColliderTag(AbyssEngine::Collider::Tag tag) { colliderTag_ = static_cast<unsigned int>(tag); }

    void SetBulletType(BulletType type) { bulletType_ = type; }

    void SetBulletSpeed(const float& speed) { bulletSpeed_ = speed; }

    void SetBeamColor(const AbyssEngine::Vector4& color) { beamColor_ = color; }
    void SetMissileColor(const AbyssEngine::Vector4& color) { missileColor_ = color; }

    void SetBeamScale(const float& scale) { beamScale_ = scale; }
    void SetMissileScale(const float& scale) { missileScale_ = scale; }

    void SetBeamWidth(const float& width) { beamWidth_ = width; }
    void SetMissileWidth(const float& width) { missileWidth_ = width; }

    void SetHomingStrength(const float& str) { homingStrength_ = str; }

    void SetIsHoming(const bool& active) { isHoming_ = active; }

    void SetTargetTag(const unsigned int& tag) { targetTag_ = tag; }

    void SetEnableMuzzleFlashParticleEffect(const bool& enable) { enableMuzzleFlashParticleEffect_ = enable; }

    void SetBeamIntensity(const float& intensity) { beamIntensity_ = intensity; }
    void SetMissileIntensity(const float& intensity) { missileIntensity_ = intensity; }

    void SetBeamParticleIntensity(const float& intensity) { beamParticleIntensity_ = intensity; }
    void SetMissileParticleIntensity(const float& intensity) { missileParticleIntensity_ = intensity; }

    void SetBeamParticleColor(const AbyssEngine::Vector4& col) { beamParticleColor_ = col; }

    void SetMissileParticleColor(const AbyssEngine::Vector4& col) { missileParticleColor_ = col; }

    void SetActiveRateOfFire(const bool& active) { activeRateOfFire_ = active; }

    void SetBulletLifespan(const float& lifespan) { bulletLifespan_ = lifespan; }

    void SetBullletAttackPoint(const float& ap) { bulletAttackPoint_ = ap; }

    void SetTargetTransform(const std::shared_ptr<AbyssEngine::Transform>& transform) { targetTransform_ = transform; }

    void SetHitParticleParam(AbyssEngine::ComputeParticleEmitter::EmitParameter param) { hitParticleParam_ = param; }
    void SetHitFireParticleParam(AbyssEngine::ComputeParticleEmitter::EmitParameter param) { hitFireParticleParam_ = param; }
    void SetTerrainHitParticleParam(AbyssEngine::ComputeParticleEmitter::EmitParameter param) { terrainHitParticleParam_ = param; }

    const std::shared_ptr<AbyssEngine::AudioSource>& GetShotSound() { return shotSound_; }

    void SetBeamSoundVolume(const float& v) { beamSoundVolume = v; }

private:
    //ADS(スコープを覗いているか)
    bool ADS_ = false;

    //銃口（弾の生成位置）
    AbyssEngine::Vector3 muzzlePos_;

    //連続撃ちに感覚を設けるか
    bool activeRateOfFire_ = true;

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

    //マズルフラッシュ用の画像6
    std::shared_ptr<AbyssEngine::BillboardRenderer> muzzleFlashComponent_;
    std::shared_ptr<AbyssEngine::BillboardRenderer> beamMuzzleFlashComponent_;
    
    //エフェクト寿命
    float flashLifespan_ = 0.0f;
    float Max_Flash_Lifespan = 0.02f;

    //弾丸の大きさ
    float bulletRadius_ = 0.1f;

    //弾丸の寿命
    float bulletLifespan_ = 2.0f;

    //弾丸の判定につけるタグ
    unsigned int colliderTag_ = 0;

    //発射する弾の種類
    BulletType bulletType_ = BulletType::Bullet;

    //ビームの幅
    float beamWidth_ = 1.0f;
    float missileWidth_ = 1.0f;
    //ビームのカラー
    AbyssEngine::Vector4 beamColor_ = {1,1,1,1};
    //ビームエフェクトの大きさ
    float beamScale_ = 1.0f;
    float missileScale_ = 1.0f;

    //ミサイルのカラー
    AbyssEngine::Vector4 missileColor_ = { 1.0f,0.4f,0.0f,1.0f };

    //ミサイルのパーティクルカラー
    AbyssEngine::Vector4 missileParticleColor_ = { 1.0f,0.4f,0.0f,1.0f };

    //発砲する弾丸の速度
    float bulletSpeed_ = 50.0f;

    //発砲時に出現させるパーティクルエフェクト
    std::shared_ptr<AbyssEngine::ComputeParticleEmitter> muzzleFlashParticleEmitter_;
    float particleAmplitudeSpeed_ = 70.0f;//パーティクルが周りに散らばる速度
    float particleSpeed_ = 200.0f;//攻撃方向へ向かう速度
    float flashParticleLifespan_ = 0.03f;
    //AbyssEngine::Vector3 muzzleFlashDirection_;//マズルフラッシュを発生させるベクトル

    bool isHoming_ = false;
    float homingStrength_ = 1.0f;

    //ビームの明るさ
    float beamIntensity_ = 4.0f;
    float missileIntensity_ = 4.0f;

    //ビームのパーティクルの明るさ
    float beamParticleIntensity_ = 30.0f;
    float missileParticleIntensity_ = 3.0f;

    //マズルフラッシュ用のパーティクルエフェクトを再生するか
    bool enableMuzzleFlashParticleEffect_ = true;

    //目標に設定するアクターのタグ
    unsigned int targetTag_;

    //ホーミングターゲット
    std::weak_ptr<AbyssEngine::Transform> targetTransform_;

    AbyssEngine::Vector4 beamParticleColor_ = { 0,0.25f,1.0f,1.0f };

    //ヒットエフェクトのパラメーターを持たせておく
    //射出時ビームアクターに設定する
    AbyssEngine::ComputeParticleEmitter::EmitParameter hitParticleParam_;
    AbyssEngine::ComputeParticleEmitter::EmitParameter hitFireParticleParam_;
    AbyssEngine::ComputeParticleEmitter::EmitParameter terrainHitParticleParam_;

    //音
    std::shared_ptr<AbyssEngine::AudioSource> shotSound_;

    float beamSoundVolume = 1.0f;

    //弾丸の攻撃力
    float bulletAttackPoint_ = 1.0f;
};

