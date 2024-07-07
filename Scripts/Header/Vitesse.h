#pragma once
#include "HumanoidWeapon.h"
#include "Camera.h"
#include "SkeletalMesh.h"
#include "StateMachine.h"
#include "VitesseConstants.h"

class ThrusterEffect;

class Vitesse : public HumanoidWeapon 
{
public:
    Vitesse();
    ~Vitesse() = default;

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
    void Update()override;
    void Move()override;
    bool DrawImGui()override;

public:
    //�s���X�e�[�g
    enum class ActionState
    {
        GMove,
        FMove,
        TakeOff,
        Landing,
    };

    //�A�j���[�V����
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

public:
    const std::unique_ptr<StateMachine<State<Vitesse>>>& GetStateMachine() { return stateMachine_; }


private:
    void UpdateInputMove()override;
    void CameraRollUpdate();

private:
    std::shared_ptr<AbyssEngine::Camera> camera_;

#if 1
    AbyssEngine::AnimBlendSpace2D* runMoveAnimation_;//����ړ�
#else
    AbyssEngine::AnimBlendSpace1D* runMoveAnimation_;//����ړ�
#endif // 0
    AbyssEngine::AnimBlendSpace2D* flyMoveAnimation_;//�󒆈ړ�

    std::unique_ptr<StateMachine<State<Vitesse>>> stateMachine_;

    //�K�v�Ȋ���̃X���X�^�[
    std::shared_ptr<ThrusterEffect> thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Installed_Units)];
    //std::shared_ptr<ThrusterEffect> thruster_;

    float cameraRollSpeed_ = 90.0f;
    

};

