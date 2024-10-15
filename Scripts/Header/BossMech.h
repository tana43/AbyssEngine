#pragma once
#include "HumanoidWeapon.h"

namespace AbyssEngine
{
    class SkeletalMesh;
    template <class T>
    class BehaviorTree;
}

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
    };


    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;


    //突進
    void RushAttackUpdate();

public:
    const std::weak_ptr<Vitesse>& GetTargetVitesse() const { return targetVitesse_; }

private:
    //コライダー設定
    void ColliderInitialize();


    void BehaviorTreeInitialize();

private:
    //std::shared_ptr<AbyssEngine::SkeletalMesh> model_;

    //ターゲット
    std::weak_ptr<Vitesse> targetVitesse_;

    //ビヘイビアツリー
    std::shared_ptr<AbyssEngine::BehaviorTree<BossMech>> aiTree_;

    //突進攻撃の誘導の強さ　
    float rushHoming_ = 1.0f;
};

