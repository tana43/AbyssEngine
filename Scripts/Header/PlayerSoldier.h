#pragma once
#include "Character.h"

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

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    void Update()override;
    bool DrawImGui()override;

    //ヴィテスに搭乗する
    void BoardingTheVitesse();

public:
    //ヴィテスに搭乗しているか
    const bool& GetVitesseOnBoard() const { return vitesseOnBoard_; }
    void SetVitesseOnBoard(const bool& flag) { vitesseOnBoard_ = flag; }

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

    std::shared_ptr<Vitesse> vitesse_;
};


