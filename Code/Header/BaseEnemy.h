#pragma once
#include "Character.h"

class BaseEnemy : public AbyssEngine::Character
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

    void UpdateBegin()override;

    void DrawDebug()override;

    //目標地点に向かって移動 目的に到達したらtrueを返す
    bool MoveToTarget();

    //引数の座標を中心に横へ移動
    void SideMove(const AbyssEngine::Vector3& centerPos, bool& moveRight);

    //ターゲット位置をランダム設定
    void SetRandomTargetPosition();

    //縄張り設定
    void SetTerritory(const AbyssEngine::Vector3& origin, float range);

public:
    const bool& GetCanAttack() const { return canAttack_; }
    void SetCanAttack(const bool& can) { canAttack_ = can; }

    const bool& GetCanDodge() const { return canDodge_; }
    void SetCanDodge(const bool& can) { canDodge_ = can; }

    const std::shared_ptr<AbyssEngine::Actor>& GetTargetActor() const { return targetActor_; }

    const AbyssEngine::Vector3& GetTargetPosition() const { return targetPosition_; }
    void SetTargetPosition(const AbyssEngine::Vector3& pos) { targetPosition_ = pos; }

    const float& GetTerritoryRange() const { return territoryRange_; }
    void SetTerritoryRange(const float& range) { territoryRange_ = range; }

    
protected:
    //目標地点
    AbyssEngine::Vector3 targetPosition_;

    //ターゲット（プレイヤー）
    std::shared_ptr<AbyssEngine::Actor> targetActor_;

    //縄張り設定
    AbyssEngine::Vector3 territoryOrigin_;
    float territoryRange_ = 3.0f;

    //目標地点に到達したかを判定するための円半径
    float goalRadius_ = 0.5f;

    //攻撃可能か
    bool canAttack_;

    //回避可能か
    bool canDodge_;
};

