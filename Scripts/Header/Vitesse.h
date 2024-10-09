#pragma once
#include "HumanoidWeapon.h"
#include "Camera.h"
#include "SkeletalMesh.h"
#include "StateMachine.h"
#include "VitesseConstants.h"

class ThrusterEffect;
class Soldier;

namespace AbyssEngine
{
    class StaticMesh;
    class AttackerSystem;
    class AttackCollidier;
}

class Vitesse : public HumanoidWeapon 
{
public:
    Vitesse();
    ~Vitesse() = default;

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
    void Update()override;
    void DrawImGui()override;

    void AnimationInitialize();

    //速度によって影響されるスラスター
    void ThrusterInfluenceVelocity();
    //スラスターをすべて停止
    void ThrusterAllStop();

    

    //回避行動
    void Dodge(AbyssEngine::Vector3 direction/*回避行動*/);

    //ステップ移動(処理内容は回避と同じ)
    void StepMove(AbyssEngine::Vector3 moveDirection, float speed);

    //攻撃判定がヒットした
    void OnCollision(const std::shared_ptr<AbyssEngine::Collider>& hitCollider, 
        AbyssEngine::Collision::IntersectionResult result)override;

public:
    //行動ステート
    enum class ActionState
    {
        GMove,
        FMove,
        TakeOff,
        Landing,
        Boarding,
        HighSpeedFlight,
        MeleeAtkDash,
        MeleeAtk,
    };

    //アニメーション
    enum class AnimationIndex
    {
        Stand,
        Run_F,
        Run_R,
        Run_L,
        Run_B,
        Flight_Idle,
        Flight_F,
        Flight_R,
        Flight_L,
        Flight_B,
        Flight_Up,
        Flight_Down,
        Landing,
        Board_Standby, //乗り込み姿勢へ
        Board_Complete,//乗り込み姿勢から立ち姿勢へ
        HighSpeedFlight_F,
        HighSpeedFlight_R,
        HighSpeedFlight_L,
        HighSpeedFlight_B,

        Dodge_FR,
        Dodge_FL,

        //地上移動時に落下した際の挙動
        Ground_Fall,

        //近接攻撃
        Slash_Dash_Start,
        Slash_Dash_Loop,
        Slash_Dash_Loop_R,
        Slash_Dash_Loop_L,
        Slash_N_1,
        Slash_N_2,
        Slash_N_2_End,
        Slash_N_3,
        Slash_N_3_End,
        Slash_R_1,

        //ブレンドモーション
        Run_Move,
        Flight_Move1D,
        Flight_Move2D,
        Flight_Move,
        HighSpeedFlight_Move2D,
        HighSpeedFlight_Move,
    };
    //アニメーションステートマシーンEnum
    enum class AnimationState
    {
        Default,
        Ground_Move,
        Flight_Move,
        HighSpeedFlight,
    };

public:
    const std::shared_ptr<AbyssEngine::StateMachine<State<Vitesse>>>& GetStateMachine() { return stateMachine_; }
    AbyssEngine::AnimBlendSpace2D* GetGroundMoveAnimation() { return groundMoveAnimation_; }
    //AbyssEngine::AnimBlendSpace2D* GetFlyMoveAnimation() { return flyMoveAnimation_; }
    AbyssEngine::AnimBlendSpaceFlyMove* GetFlightAnimation() { return flightAnimation_; }
    AbyssEngine::AnimBlendSpaceFlyMove* GetHighSpeedFlightAnimation() { return highSpeedFlightAnimation_; }
    
    const std::weak_ptr<Soldier>& GetPilot() { return pilot_; }
    void SetPilot(const std::shared_ptr<Soldier>& p) { pilot_ = p; }

    const std::shared_ptr<AbyssEngine::StaticMesh>& GetRightWeaponModel() const { return rightWeaponModel_; }

    const AbyssEngine::Vector3& GetMoveVec() const { return moveVec_; }
    void SetMoveVec(const AbyssEngine::Vector3& vec) { moveVec_ = vec; }

    const AbyssEngine::Vector3& GetMoveDirection() const { return moveDirection_; }
    void SetMoveDirection(const AbyssEngine::Vector3& vec) { moveDirection_ = vec; }

    const bool& GetCanBoarding() const { return canBoarding_; }
    void SetCanBoarding(const bool& can) { canBoarding_ = can; }

    const bool& GetActiveLockon() const { return activeLockon_; }
    void SetActiveLockon(const bool& active) { activeLockon_ = active; }

    const bool& GetChangeLockonTarget() const { return changeLockonTarget_; }

    const float& GetDodgeMaxSpeed() const { return dodgeMaxSpeed_; }
    const float& GetHighSpeedFlightMaxSpeed() const { return highSpeedFlightMaxSpeed_; }

    const float& GetMeleeAtkDashSpeed() const { return meleeAtkDashSpeed_; }
    const float& GetMeleeAtkMaxSpeed() const { return meleeAtkDashMaxSpeed_; }

    const float& GetMeleeAtkSpeed() const { return meleeAtkSpeed_; }

    const float& GetMeleeAtkRange() const { return meleeAtkRange_; }

    const std::shared_ptr<AbyssEngine::Camera>& GetCamera() const { return camera_; }

    const float& GetDefaultCameraLagSpeed() const { return defaultCameraLagSpeed_; }

    const std::weak_ptr<AbyssEngine::Actor>& GetLockonTarget() const { return lockonTarget_; }

    const std::shared_ptr<AbyssEngine::AttackerSystem>& GetAttackerSystem() const { return attackerSystem_; }

    //ターゲットまでのベクトルを算出
    //ターゲットがいない場合は見ている方向を返す
    AbyssEngine::Vector3 ToTarget();

    void ChangeActionState(const ActionState& state);
    void ChangeAnimationState(const AnimationState& state);
    

    //パイロット乗り込み
    void GetOnBoardPilot(const std::shared_ptr<Soldier>& pilot);

    //移動入力の反映
    void UpdateInputMove()override;

    //地上モードへ移行
    void ToGroundMode()override;

    //ターゲットのコライダーを補足する
    void TargetAcquisition();

    //アニメーション再生
    void PlayAnimation(AnimationIndex index,float transTime = 0.1f,float startTime = 0.0f);

    //ラジアルブラーをターゲットがいる所を中心として演出させる
    void RadialBlurFromTarget();

private:
    void CameraRollUpdate();

    //スラスター更新処理
    void ThrusterUpdate();

    //パイロットが搭乗しているかを判定するとこ
    void PilotUpdate();

    //上昇の入力を反映させる
    void RiseInputUpdate();

    //コライダー設定
    void ColliderInitialize();

    //アタッカー設定(コライダーを設定した後がいいかも)
    void AttackerInitialize();
    
private:
    std::shared_ptr<AbyssEngine::Camera> camera_;

#if 1
    AbyssEngine::AnimBlendSpace2D* groundMoveAnimation_;//走り移動
#else
    AbyssEngine::AnimBlendSpace1D* runMoveAnimation_;//走り移動
#endif // 0
    //AbyssEngine::AnimBlendSpace2D* flyMoveAnimation_;//空中移動
    AbyssEngine::AnimBlendSpaceFlyMove* flightAnimation_;//空中移動
    AbyssEngine::AnimBlendSpaceFlyMove* highSpeedFlightAnimation_;//高速空中移動

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

    //回避行動
    AbyssEngine::Vector3 dodgeDirection_ = {0,0,0};
    //回避速度
    float dodgeSpeed_ = 100.0f;
    float dodgeMaxSpeed_ = 100.0f;

    //高速飛行移動速度
    float highSpeedFlightMaxSpeed_ = 50.0f;

    //カメラのデフォルト値
    float defaultCameraLagSpeed_ = 0.0f;
    
    //右手武器
    std::shared_ptr<AbyssEngine::StaticMesh> rightWeaponModel_;
    //左手武器
    std::shared_ptr<AbyssEngine::StaticMesh> leftWeaponModel_;

    struct SocketOffset
    {
        AbyssEngine::Vector3 pos;
        AbyssEngine::Vector3 rot;
    };
    const SocketOffset Left_Weapon_Offset =
    {
        {-42.650f,-5.65f,-20.8f},
        {-21.2f,180.0f,95.2f},
    };
    const SocketOffset Right_Weapon_Offset =
    {
        {42.650f,5.65f,20.8f},
        {159.8f,180.0f,95.2f},
    };

    //ロックオンしているか
    bool activeLockon_ = false;
    //補足、もしくはロックオン可能になる範囲
    float lockRadius_ = 200.0f;

    //目標となるアクター
    std::weak_ptr<AbyssEngine::Actor> lockonTarget_;

    //ロックオンしているターゲットが変更されたときにtrue
    bool changeLockonTarget_ = false;

    //近接攻撃をする際に敵に近寄る速度
    float meleeAtkDashSpeed_ = 70.0f;
    float meleeAtkDashMaxSpeed_ = 70.0f;

    //近接攻撃が可能になる範囲
    float meleeAtkRange_ = 25.0f;

    //近接攻撃中の速度
    float meleeAtkSpeed_ = 10.0f;

    //アタッカーシステム
    std::shared_ptr<AbyssEngine::AttackerSystem> attackerSystem_;

    //アタックコライダー
    std::vector<std::shared_ptr<AbyssEngine::AttackCollider>> lWeaponAtkColliderList_;
    std::vector<std::shared_ptr<AbyssEngine::AttackCollider>> rWeaponAtkColliderList_;
};

