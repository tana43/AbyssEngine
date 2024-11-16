#pragma once
#include "HumanoidWeapon.h"
#include <vector>

namespace AbyssEngine
{
    class SkeletalMesh;
    template <class T>
    class BehaviorTree;
    class AttackerSystem;
    class AttackCollider;
}

class Gun;

class Vitesse;

class BossMech : public HumanoidWeapon
{
public:
    enum class AnimationIndex
    {
        Idle,
        Run,
        Jump_Start,
        Jump_Loop,
        Jump_End,
        Crouching,//クラウチング

        Fly_Idle,

        Skill_01,//ビーム攻撃用

        Combo_01,//近接攻撃
        Combo_02,//近接攻撃
        Combo_03,//近接攻撃

        Fly_Front_Start,
        Fly_Front_Loop,
        Fly_Front_End,
    };

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

    void Update()override;

    //突進
    void RushAttackUpdate();

    //誘導するビームを生成
    bool ShotHomingBeam();

    //目標座標まで移動する 移動が完了したかを返す
    bool MoveTo(AbyssEngine::Vector3 goalPos);

    void DrawImGui()override;

public:
    const std::weak_ptr<Vitesse>& GetTargetVitesse() const { return targetVitesse_; }

    const std::shared_ptr<AbyssEngine::AttackerSystem>& GetAttackerSystem() const { return attackerSystem_; }

private:
    //コライダー設定
    void ColliderInitialize();

    //ビヘイビアツリー初期化
    void BehaviorTreeInitialize();

    //アタッカーシステム初期化
    void AttackerSystemInitialize();

    void UpdateMuzzlePos();

private:
    //std::shared_ptr<AbyssEngine::SkeletalMesh> model_;

    //ターゲット
    std::weak_ptr<Vitesse> targetVitesse_;

    //ビヘイビアツリー
    std::shared_ptr<AbyssEngine::BehaviorTree<BossMech>> aiTree_;

    //アタッカーシステム
    //近接攻撃の当たり判定などを管理する
    std::shared_ptr<AbyssEngine::AttackerSystem> attackerSystem_;

    //攻撃判定用コライダー
    std::vector<std::shared_ptr<AbyssEngine::AttackCollider>> attackColliders_;

    //突進攻撃の誘導の強さ　
    float rushHoming_ = 1.0f;

    //通常のホーミング弾を撃つガンコンポーネント
    std::shared_ptr<Gun> noramlHomingGunL_;

    //ホーミングが強い弾を撃つ
    std::shared_ptr<Gun> superHomingGunL_;

    //一定時間経過後にホーミングする弾を撃つ
    std::shared_ptr<Gun> delayHomingGunL_;

    //自分の中心となるコライダー
    std::weak_ptr<AbyssEngine::Actor> coreCollider_;

    //射撃方向を計算するようのタイマー
    //float shotDireTimer_;

    //一度に撃つビームの数
    int shotHomingBeamCount_ = 50;

    float beamShotTimer_;

    float shotBeamActionCooldown_;

    //目標地点に到達したとみなす判定距離
    float nearRange_ = 5.0f;
    
};

