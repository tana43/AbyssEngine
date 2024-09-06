#pragma once
#include "BaseEnemy.h"

namespace AbyssEngine
{
    template <class T>
    class BehaviorTree;
    class SkeletalMesh;
    class Animator;
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

    bool DrawImGui()override;
    void DrawDebug()override;

    //プレイヤーを索敵
    bool SearchTarget();

    //エイム位置を更新
    void LockOn();

    //射撃
    void Shot();

    //攻撃
    void ReloadUpdate();

public:
    const std::shared_ptr<AbyssEngine::SkeletalMesh>& GetModel() const { return model_; }
    const std::shared_ptr<AbyssEngine::Animator>& GetAnimator() const;

    const float& GetLockOnTime() const { return LockOn_Time; }
    const float& GetLockOnShotTime() const { return LockOn_Shot_Time; }
    const float& GetAtkReloadTime() const { return Atk_Reload_Time; }
    
    const bool& GetIsTargetFind() const { return isTargetFind_; }
private:
    std::shared_ptr<AbyssEngine::SkeletalMesh> model_;

    //ビヘイビアツリー
    std::shared_ptr<AbyssEngine::BehaviorTree<BotEnemy>> aiTree_;

    //索敵範囲
    float searchAreaRadius_ = 5.0f;

    //疑似的な視野角　敵が視界内にいるかを判断する
    float degreeFov_ = 150.0f;

    //上の視野角を０～１に正規化したもの
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
};

