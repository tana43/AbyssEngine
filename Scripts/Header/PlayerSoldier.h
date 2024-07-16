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

    //���B�e�X�ɓ��悷��
    bool BoardingTheVitesse();
    //���B�e�X����\�������𔻒�
    void BoardingDistanceJudge(const float& range/*����\����*/);

    //�ړ�����
    void InputMove();
public:
    //���B�e�X�ɓ��悵�Ă��邩
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
    float baseAcceleration_ = 2.0f;//��ƂȂ�����x�@����ɃR���g���[���[�̓��͒l�����v�Z������ۂ̉����x�����܂�
    const float Walk_Max_Speed = 1.0f;
    const float Run_Max_Speed = 1.5f;

    float cameraRollSpeed_ = 90.0f;

    float jumpPower_ = 10.0f;

    bool vitesseOnBoard_ = false;//���B�e�X�ɏ���Ă��邩
    bool canBoarding_ = false;//����\��

    //���B�e�X
    std::shared_ptr<Vitesse> vitesse_;

    //�X�e�[�g�}�V��
    std::unique_ptr<StateMachine<State<Soldier>>> stateMachine_;

    //���B�e�X����\����
    float boardingDistance_ = 5.0f;
};


