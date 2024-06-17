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
        Stand,
        Run_F,
        Run_R,
        Run_L,
        Run_B,
        Fly_Idle,
        Fly_F,
        Fly_R,
        Fly_L,
        Fly_B,
        Fly_Up,
        Run_Move,
        Fly_Move,
    };

private:
    void UpdateInputMove()override;
    void CameraRollUpdate();

private:
    std::shared_ptr<AbyssEngine::Camera> camera_;

    AbyssEngine::AnimBlendSpace1D* runMoveAnimation_;//‘–‚èˆÚ“®
    AbyssEngine::AnimBlendSpace1D* flyMoveAnimation_;//‹ó’†ˆÚ“®

    float cameraRollSpeed_ = 90.0f;
};

