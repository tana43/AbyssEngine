#pragma once
#include "Character.h"
#include "StateMachine.h"

class Vitesse;

namespace AbyssEngine
{
    class Camera;
    class StaticMesh;
}

class Soldier final : public AbyssEngine::Character
{
public:

    enum class AnimState
    {
        Idle,
        Walk,
        Run,
        Move,
    };

    enum class ActionState
    {
        Move,
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
public:
    //ヴィテスに搭乗しているか
    const bool& GetVitesseOnBoard() const { return vitesseOnBoard_; }
    void SetVitesseOnBoard(const bool& flag) { vitesseOnBoard_ = flag; }

    const std::shared_ptr<Vitesse>& GetMyVitesse() const { return vitesse_; }
    void SetMyVitesse(const std::shared_ptr<Vitesse>& vitesse) { vitesse_ = vitesse; }

    const bool& GetCanBoarding() const { return canBoarding_; }

private:
    void MoveUpdate();
    void CameraRollUpdate();


private:
    std::shared_ptr<AbyssEngine::Camera> camera_;

    std::shared_ptr<AbyssEngine::StaticMesh> weaponModel_;

private:
    float baseAcceleration_ = 2.0f;//基準となる加速度　これにコントローラーの入力値等が計算され実際の加速度が決まる
    const float Walk_Max_Speed = 1.0f;
    const float Run_Max_Speed = 1.5f;

    float cameraRollSpeed_ = 90.0f;

    float jumpPower_ = 10.0f;

    bool vitesseOnBoard_ = false;//ヴィテスに乗っているか
    bool canBoarding_ = false;//搭乗可能か

    //ヴィテス
    std::shared_ptr<Vitesse> vitesse_;

    //ステートマシン
    std::unique_ptr<StateMachine<State<Soldier>>> stateMachine_;

    //ヴィテス搭乗可能距離
    float boardingDistance_ = 5.0f;
};


