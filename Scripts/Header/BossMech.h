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
    //�R���C�_�[�ݒ�
    void ColliderInitialize();

private:
    std::shared_ptr<AbyssEngine::SkeletalMesh> model_;
};

