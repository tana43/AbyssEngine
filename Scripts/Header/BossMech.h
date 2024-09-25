#pragma once
#include "HumanoidWeapon.h"

class BossMech : public HumanoidWeapon
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
};

