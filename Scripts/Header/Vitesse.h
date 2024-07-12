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

    //速度によって影響されるスラスター
    void ThrusterInfluenceVelocity();
    //スラスターをすべて停止
    void ThrusterAllStop();

public:
    //行動ステート
    enum class ActionState
    {
        GMove,
        FMove,
        TakeOff,
        Landing,
    };

    //アニメーション
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
        Fly_Down,
        Landing,
        Run_Move,
        Fly_Move1D,
        Fly_Move2D,
        Fly_Move,
    };

public:
    const std::unique_ptr<StateMachine<State<Vitesse>>>& GetStateMachine() { return stateMachine_; }
    AbyssEngine::AnimBlendSpace2D* GetGroundMoveAnimation() { return groundMoveAnimation_; }
    //AbyssEngine::AnimBlendSpace2D* GetFlyMoveAnimation() { return flyMoveAnimation_; }
    AbyssEngine::AnimBlendSpaceFlyMove* GetFlyMoveAnimation() { return flyMoveAnimation_; }
    
    

private:
    void UpdateInputMove()override;
    void CameraRollUpdate();

    //スラスター更新処理
    void ThrusterUpdate();

private:
    std::shared_ptr<AbyssEngine::Camera> camera_;

#if 1
    AbyssEngine::AnimBlendSpace2D* groundMoveAnimation_;//走り移動
#else
    AbyssEngine::AnimBlendSpace1D* runMoveAnimation_;//走り移動
#endif // 0
    //AbyssEngine::AnimBlendSpace2D* flyMoveAnimation_;//空中移動
    AbyssEngine::AnimBlendSpaceFlyMove* flyMoveAnimation_;//空中移動

    std::unique_ptr<StateMachine<State<Vitesse>>> stateMachine_;
    std::unique_ptr<StateMachine<State<AbyssEngine::Animator>>> animStateMachine_;

    //必要な基数分のスラスター
    std::shared_ptr<ThrusterEffect> thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Installed_Units)];
    //std::shared_ptr<ThrusterEffect> thruster_;

    float cameraRollSpeed_ = 90.0f;
    
    //今向いている方向と速度が働いている方向差分
    AbyssEngine::Vector3 moveDirection_ = { 0,0,0 };

    //スラスターを噴射させるか
    bool activeThruster_ = true;

};

