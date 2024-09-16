#pragma once
#include "HumanoidWeapon.h"
#include "Camera.h"
#include "SkeletalMesh.h"
#include "StateMachine.h"
#include "VitesseConstants.h"

class ThrusterEffect;
class Soldier;

class Vitesse : public HumanoidWeapon 
{
public:
    Vitesse();
    ~Vitesse() = default;

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
    void Update()override;
    void DrawImGui()override;

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
        Boarding,
    };

    //アニメーション
    enum class AnimationIndex
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
        Board_Standby, //乗り込み姿勢へ
        Board_Complete,//乗り込み姿勢から立ち姿勢へ
        Run_Move,
        Fly_Move1D,
        Fly_Move2D,
        Fly_Move,
    };
    //アニメーションステートマシーンEnum
    enum class AnimationState
    {
        Default,
        Ground_Move,
        Fly_Move,
    };

public:
    const std::shared_ptr<AbyssEngine::StateMachine<State<Vitesse>>>& GetStateMachine() { return stateMachine_; }
    AbyssEngine::AnimBlendSpace2D* GetGroundMoveAnimation() { return groundMoveAnimation_; }
    //AbyssEngine::AnimBlendSpace2D* GetFlyMoveAnimation() { return flyMoveAnimation_; }
    AbyssEngine::AnimBlendSpaceFlyMove* GetFlyMoveAnimation() { return flyMoveAnimation_; }
    
    const std::weak_ptr<Soldier>& GetPilot() { return pilot_; }
    void SetPilot(const std::shared_ptr<Soldier>& p) { pilot_ = p; }

    const AbyssEngine::Vector3& GetMoveVec() const { return moveVec_; }
    void SetMoveVec(const AbyssEngine::Vector3& vec) { moveVec_ = vec; }

    const AbyssEngine::Vector3& GetMoveDirection() const { return moveDirection_; }
    void SetMoveDirection(const AbyssEngine::Vector3& vec) { moveDirection_ = vec; }

    const bool& GetCanBoarding() const { return canBoarding_; }
    void SetCanBoarding(const bool& can) { canBoarding_ = can; }

    void ChangeState(const ActionState& state);
    void ChangeAnimationState(const AnimationState& state);

    //パイロット乗り込み
    void GetOnBoardPilot(const std::shared_ptr<Soldier>& pilot);

    //移動入力の反映
    void UpdateInputMove()override;

    //地上モードへ移行
    void ToGroundMode()override;

private:
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

    std::shared_ptr<AbyssEngine::StateMachine<State<Vitesse>>> stateMachine_;
    std::shared_ptr<AbyssEngine::StateMachine<State<AbyssEngine::Animator>>> animStateMachine_;

    //必要な基数分のスラスター
    std::shared_ptr<ThrusterEffect> thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Installed_Units)];
    //std::shared_ptr<ThrusterEffect> thruster_;

    float cameraRollSpeed_ = 90.0f;
    
    //今向いている方向と速度が働いている方向差分
    AbyssEngine::Vector3 moveDirection_ = { 0,0,0 };

    //スラスターを噴射させるか
    bool activeThruster_ = true;

    //パイロット(プレイヤー)
    std::weak_ptr<Soldier> pilot_;

    //乗り込み可能な状態か
    bool canBoarding_ = false;
};

