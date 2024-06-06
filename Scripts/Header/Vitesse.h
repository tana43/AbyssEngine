#pragma once
#include "HumanoidWeapon.h"
#include "Camera.h"
#include "SkeletalMesh.h"

class Vitesse : public HumanoidWeapon 
{
public:
    Vitesse();
    ~Vitesse() = default;

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
    //void Update()override;
    void Move()override;

public:
    enum class AnimState
    {
        Idle,
        FlyFront,
        FlyRight,
        FlyLeft,
        SkyIdle,
        Jump,
        Move,
    };

private:
    void UpdateInputMove()override;

private:
    std::shared_ptr<AbyssEngine::Camera> camera_;
};

