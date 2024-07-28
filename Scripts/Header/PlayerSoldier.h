#pragma once
#include "Human.h"
#include "StateMachine.h"
#include "FadeSystem.h"

class Vitesse;

namespace AbyssEngine
{
    class Camera;
    class StaticMesh;
}

class Gun;

class Soldier final : public Human
{
public:
    //武器のオフセット回転と位置
    struct SocketOffset
    {
        AbyssEngine::Vector3 pos;
        AbyssEngine::Vector3 rot;
    };
    const SocketOffset Weapon_Offset_Move  = { { 3.35f,-4.75f,6.3f } ,{2.15f,168.7f,-99.25f} };
    const SocketOffset Weapon_Offset_Aim   = { {-10.85f,3.15f,31.75f}, {-56.85f,206.4,-47.7f} };

    enum class AnimState
    {
        Idle,
        Walk,
        Run,
        Aim,
        Jump,
        Fall_Loop,
        Land,
        Move,
    };

    enum class ActionState
    {
        Move,
        Aim,
        Jump,
    };

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    void Update()override;
    bool DrawImGui()override;

    //ヴィテスに搭乗する
    bool BoardingTheVitesse();
    //ヴィテス搭乗可能距離かを判定
    void BoardingDistanceJudge(const float& range/*搭乗可能距離*/);

    //移動操作
    void InputMove();

    //ソケット位置の変更
    void ChangeSocketTransformLinear(
        const float& changeTime,
        const AbyssEngine::Vector3& pos,
        const AbyssEngine::Vector3& rot);

    //射撃
    void GunShot();

    //ダッシュ判定
    void DashDecision();
public:
    //ヴィテスに搭乗しているか
    const bool& GetVitesseOnBoard() const { return vitesseOnBoard_; }
    void SetVitesseOnBoard(const bool& flag) { vitesseOnBoard_ = flag; }

    const std::shared_ptr<Vitesse>& GetMyVitesse() const { return vitesse_; }
    void SetMyVitesse(const std::shared_ptr<Vitesse>& vitesse) { vitesse_ = vitesse; }

    const bool& GetCanBoarding() const { return canBoarding_; }

    std::unique_ptr<StateMachine<State<Soldier>>>& GetStateMachine() { return stateMachine_; }

    const std::shared_ptr<AbyssEngine::Camera>& GetCamera() { return camera_; }

    void SetCanJump(const bool& can) { canJump_ = can; }

private:
    void MoveUpdate();
    void CameraRollUpdate();

    void SocketUpdate();

    //銃口の位置更新
    void MuzzlePosUpdate();
private:
    std::shared_ptr<AbyssEngine::Camera> camera_;

    std::shared_ptr<AbyssEngine::StaticMesh> weaponModel_;
    std::shared_ptr<Gun> gunComponent_;

private:
    float baseAcceleration_ = 2.0f;//基準となる加速度　これにコントローラーの入力値等が計算され実際の加速度が決まる
    const float Walk_Max_Speed = 1.0f;
    const float Run_Max_Speed = 1.5f;

    float cameraRollSpeed_ = 90.0f;

    float jumpPower_ = 5.0f;

    bool vitesseOnBoard_ = false;//ヴィテスに乗っているか
    bool canBoarding_ = false;//搭乗可能か
    bool canJump_ = true;//ジャンプ可能か

    //ヴィテス
    std::shared_ptr<Vitesse> vitesse_;

    //ステートマシン
    std::unique_ptr<StateMachine<State<Soldier>>> stateMachine_;

    //ヴィテス搭乗可能距離
    float boardingDistance_ = 5.0f;

    //ソケット位置を変更するための補完情報
    std::unique_ptr<AbyssEngine::FadeSystem> socketFade_;
};


