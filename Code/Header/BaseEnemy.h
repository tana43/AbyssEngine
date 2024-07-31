#pragma once
#include "Character.h"

class BaseEnemy : public AbyssEngine::Character
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

    void DrawDebug()override;

    //目標地点に向かって移動
    void MoveToTarget();

    //ターゲット位置をランダム設定
    void SetRandomTargetPosition();

    //縄張り設定
    void SetTerritory(const AbyssEngine::Vector3& origin, float range);

protected:
    //目標地点
    AbyssEngine::Vector3 targetPosition_;

    //縄張り設定
    AbyssEngine::Vector3 territoryOrigin_;
    float territoryRange_ = 3.0f;
};

