#pragma once
#include "HumanoidWeapon.h"

namespace AbyssEngine
{
    class SkeletalMesh;
}

class BossMech : public HumanoidWeapon
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

private:
    //コライダー設定
    void ColliderInitialize();

private:
    std::shared_ptr<AbyssEngine::SkeletalMesh> model_;
};

