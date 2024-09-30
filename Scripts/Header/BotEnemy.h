#pragma once
#include "BaseEnemy.h"

namespace AbyssEngine
{
    template <class T>
    class BehaviorTree;
    class SkeletalMesh;
    class Animator;
    class SphereCollider;
    class HitCollider;
}

class Gun;

class BotEnemy : public BaseEnemy
{
public:
    enum class AnimState
    {
        Idle,
        Rolling,
        Walk,
        Jump, 
        Search,
        Attack,
        Attack_Assult,//前進しつつ攻撃
    };

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
    void BehaviorTreeInitialize();

    void Update()override;

    void DrawImGui()override;
    void DrawDebug()override;

    //ターゲットを索敵
    bool SearchTarget();

    //ターゲットとの距離を判定
    //引数以内にターゲットがいるならtrue
    bool SearchTargetWithimRange(float range);

    //エイム位置を更新
    void LockOn();

    //射撃
    void Shot();

    //攻撃
    void ReloadUpdate();

    //当たり判定の初期化
    void ColliderInitialize();

public:
    const std::shared_ptr<AbyssEngine::SkeletalMesh>& GetModel() const { return model_; }
    const std::shared_ptr<AbyssEngine::Animator>& GetAnimator() const;

    const float& GetLockOnTime() const { return LockOn_Time; }
    const float& GetLockOnShotTime() const { return LockOn_Shot_Time; }
    const float& GetAtkReloadTime() const { return Atk_Reload_Time; }
    
    const bool& GetIsTargetFind() const { return isTargetFind_; }
    const bool& GetReloadNow() const { return reloadNow_; }
private:
    std::shared_ptr<AbyssEngine::SkeletalMesh> model_;

    //ビヘイビアツリー
    std::shared_ptr<AbyssEngine::BehaviorTree<BotEnemy>> aiTree_;

    //索敵範囲
    float searchAreaRadius_ = 5.0f;

    //疑似的な視野角　敵が視界内にいるかを判断する
    float degreeFov_ = 150.0f;

    //上の視野角を０〜１に正規化したもの
    float fov_;

    //狙っている位置
    AbyssEngine::Vector3 aimPosition_;

    //銃コンポーネント
    std::shared_ptr<Gun> gunComponent_;

    //ロックオン中の時間
    float LockOn_Time = 4.0f;

    //ロックオン完了から発砲に掛かる時間
    float LockOn_Shot_Time = 1.0f;

    //再度攻撃できるのに掛かる時間
    float Atk_Reload_Time = 3.5f;

    //リロード時間の計測用タイマー
    float reloadTimer_ = 0;

    //現在リロードしているか
    bool reloadNow_ = false;

    //プレイヤーを見つけているか
    bool isTargetFind_ = false;

    std::shared_ptr<AbyssEngine::HitCollider> hitCollider_;
};

