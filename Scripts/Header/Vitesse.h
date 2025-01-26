#pragma once
#include "HumanoidWeapon.h"
#include "Camera.h"
#include "SkeletalMesh.h"
#include "StateMachine.h"
#include "VitesseConstants.h"

class ThrusterEffect;
class Soldier;
class Gun;
template<class T>
class ComboSystem;

namespace AbyssEngine
{
    class StaticMesh;
    class AttackerSystem;
    class AttackCollider;
    class ComputeParticleEmitter;
    class AudioSource;
    class SwordTrailRenderer;
}

class Vitesse : public HumanoidWeapon
{
public:
    Vitesse();
    ~Vitesse() = default;

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
    void Update()override;
    void UpdateEnd()override;
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

    //ソードトレイルのオンオフ
    void ActiveSwordTrail(bool active);

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
        Flinch,
        Aiming,
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

        //ひるみ
        Flinch,

        Shot_Pose,

        Shot_Move_F,
        Shot_Move_R,
        Shot_Move_L,
        Shot_Move_B,

        //追加のアセットモーションはここより上で----------------------------------------------------------------------------

        //ブレンドモーション
        Run_Move,
        Flight_Move1D,
        Flight_Move2D,
        Flight_Move,
        HighSpeedFlight_Move2D,
        HighSpeedFlight_Move,

        //Aim_IK,
        Aim_Move,
    };
    //アニメーションステートマシーンEnum
    enum class AnimationState
    {
        Default,
        Ground_Move,
        Flight_Move,
        HighSpeedFlight,
        Aim_Move,
    };

public:
    const std::shared_ptr<AbyssEngine::StateMachine<State<Vitesse>>>& GetStateMachine() { return stateMachine_; }
    AbyssEngine::AnimBlendSpace2D* GetGroundMoveAnimation() { return groundMoveAnimation_; }
    //AbyssEngine::AnimBlendSpace2D* GetFlyMoveAnimation() { return flyMoveAnimation_; }
    AbyssEngine::AnimBlendSpaceFlyMove* GetFlightAnimation() { return flightAnimation_; }
    AbyssEngine::AnimBlendSpaceFlyMove* GetHighSpeedFlightAnimation() { return highSpeedFlightAnimation_; }
    AbyssEngine::AnimAiming* GetAimingAnimation() { return aimingAnimation_; }
    
    const std::weak_ptr<Soldier>& GetPilot() { return pilot_; }
    void SetPilot(const std::shared_ptr<Soldier>& p) { pilot_ = p; }

    const std::shared_ptr<AbyssEngine::StaticMesh>& GetRightWeaponModel() const { return rightWeaponModel_; }

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

    const AbyssEngine::Vector3& GetDefaultCameraLagSpeed() const { return defaultCameraLagSpeed_; }

    const std::weak_ptr<AbyssEngine::Actor>& GetLockonTarget() const { return lockonTarget_; }

    const std::shared_ptr<AbyssEngine::AttackerSystem>& GetAttackerSystem() const { return attackerSystem_; }

    const AbyssEngine::Vector3& GetAimTargetPosition() const { return aimTargetPos_; }

    const float& GetBoostAmount() const { return boostAmount_; }
    const float& GetMaxBoostAmount() const { return Max_Boost_Amount; }

    const float& GetDodgeBoostCost() const { return dodgeBoostCost_; }
    const float& GetDashBoostCostS() const { return dashBoostCostS_; }
    const float& GetMeleeBoostCostS() const { return meleeBoostCostS_; }

    const bool& GetIsBoostOverHeat() const { return isBoostOverHeat_; }

    const std::shared_ptr<AbyssEngine::ComputeParticleEmitter>& GetDodgeParticleEffect() const { return dodgeParticleEffect_; }

    void SetIsSystemStart(const bool& flag) { isSystemStart_ = flag; }

    const std::shared_ptr<AbyssEngine::AudioSource>& GetMeleeBoostSound() { return meleeBoostSound_; }
    const std::shared_ptr<AbyssEngine::AudioSource>& GetBoostBurstSound() { return boostBurstSound_; }

    const std::shared_ptr<ComboSystem<Vitesse>>& GetComboSystem() { return comboSystem_; }


    void BeginAttack(const std::string& name);

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

    //飛行モードへ移行
    void ToFlightMode()override;

    //ターゲットのコライダーを補足する
    void TargetAcquisition();

    //アニメーション再生
    void PlayAnimation(AnimationIndex index,float* transTime = nullptr,float startTime = 0.0f);

    //ラジアルブラーをターゲットがいる所を中心として演出させる
    void RadialBlurFromTarget();

    //ダメージを喰らう
    bool ApplyDamage(const AttackParameter& param, DamageResult* damageResult = nullptr)override;

    //怯み
    void Flinch(AbyssEngine::StaggerType type)override;

    //カメラの正面を向くようにキャラを回転させる
    void RotateToFront();

    //カメラやロックオンの状況から狙っている位置を算出、更新する
    void UpdateShotTarget();

    //ビーム攻撃
    void BeamShot();

    //引数分ブーストゲージ(max100)を減らす ブーストが足りないならfalseを返す
    bool UseBoostGauge(float useBoostAmount);

private:
    void CameraRollUpdate();

    //スラスター更新処理
    void ThrusterUpdate();

    //パイロットが搭乗しているかを判定するとこ
    void PilotUpdate();

    //上昇の入力を反映させる
    void RiseInputUpdate();

    //コライダー初期化
    void ColliderInitialize();

    //アタッカー初期化(コライダーを設定した後がいいかも)
    void AttackerInitialize();

    //コンボ関係の初期化
    void ComboInitialize();

    void AimIKTest();

    //銃口の位置更新
    void UpdateGunMuzzlePos();
    
    //ビーム攻撃
    bool BeamShotByComponent(Gun& gun,AbyssEngine::Vector3 targetPosition, AbyssEngine::Vector3* terrainHitPosition = nullptr);

    //ガンコンポーネントを初期化
    void GunInitialize(Gun& gun);

    //ブーストゲージ更新
    void UpdateBoostGauge();

    //エミッシブで起動を表現する
    void UpdateEmissive();

    //効果音初期化
    void AudioInitialize();

    //ソードトレイルの位置更新
    void UpdateSwordTrailPos();

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

    //AbyssEngine::AnimAimIK* aimIKAnimation_;//エイムIK
    AbyssEngine::AnimAiming* aimingAnimation_;//移動付きのエイムモーション

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
    float dodgeSpeed_ = 110.0f;
    float dodgeMaxSpeed_ = 150.0f;

    //高速飛行移動速度
    float highSpeedFlightMaxSpeed_ = 50.0f;

    //カメラのデフォルト値
    AbyssEngine::Vector3 defaultCameraLagSpeed_ = { 0,0,0 };
    
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
    float meleeAtkRange_ = 35.0f;

    //近接攻撃中の速度
    float meleeAtkSpeed_ = 10.0f;

    //アタッカーシステム
    std::shared_ptr<AbyssEngine::AttackerSystem> attackerSystem_;

    //アタックコライダー
    std::vector<std::shared_ptr<AbyssEngine::AttackCollider>> lWeaponAtkColliderList_;
    std::vector<std::shared_ptr<AbyssEngine::AttackCollider>> rWeaponAtkColliderList_;

    //銃コンポーネント
    std::shared_ptr<Gun> gunComponentR_;
    std::shared_ptr<Gun> gunComponentL_;
    //銃口のローカル座標
    AbyssEngine::Vector3 muzzleOffsetPosR_ = {0.0f,-0.05f,0.7f};
    AbyssEngine::Vector3 muzzleOffsetPosL_ = {0.0f,0.05f,0.7f};

    //カメラの位置やロックオンの状況から狙っている位置を持たせておく
    AbyssEngine::Vector3 aimTargetPos_;

    //ブーストゲージ
    float Max_Boost_Amount = 100.0f;
    float boostAmount_ = Max_Boost_Amount;
    float boostHealAmount_ = 25.0f;//ブーストが１秒間に回復する量
    float boostOverHeatHealAmount_ = 45.0f;//オーバーヒート時にブーストが１秒間に回復する量
    float boostHealStartTime_ = 0.5f;//最後にブーストを使ってから回復し始めるまでのタイム
    float boostTimer_ = 0.0f;
    bool isBoostOverHeat_ = false;

    //消費ブースト
    float dodgeBoostCost_ = 8.0f;//瞬間
    float dashBoostCostS_ = 10.0f;//毎秒
    float meleeBoostCostS_ = 7.0f;//近接攻撃時

    std::shared_ptr<AbyssEngine::ComputeParticleEmitter> dodgeParticleEffect_;

    std::shared_ptr<AbyssEngine::AudioSource> changeTargetSound_;
    std::shared_ptr<AbyssEngine::AudioSource> systemStartUpSound_;
    std::shared_ptr<AbyssEngine::AudioSource> boostAlwaysSound_;
    std::shared_ptr<AbyssEngine::AudioSource> boostBurstSound_;
    std::shared_ptr<AbyssEngine::AudioSource> boostAlertSound_;//ブーストが切れたときに鳴らすSE
    std::shared_ptr<AbyssEngine::AudioSource> boostFullSound_;//ブーストが満タンになったときにSE
    std::shared_ptr<AbyssEngine::AudioSource> meleeBoostSound_;//近接攻撃前のブーストSE

    //システム起動演出
    //エミッシブ強度
    float initEmissiveIntensity_;
    float sSUTimer_ = 0.0f;
    float sSUTime_ = 0.5f;
    bool isSystemStart_ = false;

    //ソードトレイル
    std::shared_ptr<AbyssEngine::SwordTrailRenderer> swordTrailR_;
    std::shared_ptr<AbyssEngine::SwordTrailRenderer> swordTrailL_;

    //コンボシステム
    std::shared_ptr<ComboSystem<Vitesse>> comboSystem_;
};

