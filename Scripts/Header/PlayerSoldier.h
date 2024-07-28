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
    //����̃I�t�Z�b�g��]�ƈʒu
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

    //���B�e�X�ɓ��悷��
    bool BoardingTheVitesse();
    //���B�e�X����\�������𔻒�
    void BoardingDistanceJudge(const float& range/*����\����*/);

    //�ړ�����
    void InputMove();

    //�\�P�b�g�ʒu�̕ύX
    void ChangeSocketTransformLinear(
        const float& changeTime,
        const AbyssEngine::Vector3& pos,
        const AbyssEngine::Vector3& rot);

    //�ˌ�
    void GunShot();

    //�_�b�V������
    void DashDecision();
public:
    //���B�e�X�ɓ��悵�Ă��邩
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

    //�e���̈ʒu�X�V
    void MuzzlePosUpdate();
private:
    std::shared_ptr<AbyssEngine::Camera> camera_;

    std::shared_ptr<AbyssEngine::StaticMesh> weaponModel_;
    std::shared_ptr<Gun> gunComponent_;

private:
    float baseAcceleration_ = 2.0f;//��ƂȂ�����x�@����ɃR���g���[���[�̓��͒l�����v�Z������ۂ̉����x�����܂�
    const float Walk_Max_Speed = 1.0f;
    const float Run_Max_Speed = 1.5f;

    float cameraRollSpeed_ = 90.0f;

    float jumpPower_ = 5.0f;

    bool vitesseOnBoard_ = false;//���B�e�X�ɏ���Ă��邩
    bool canBoarding_ = false;//����\��
    bool canJump_ = true;//�W�����v�\��

    //���B�e�X
    std::shared_ptr<Vitesse> vitesse_;

    //�X�e�[�g�}�V��
    std::unique_ptr<StateMachine<State<Soldier>>> stateMachine_;

    //���B�e�X����\����
    float boardingDistance_ = 5.0f;

    //�\�P�b�g�ʒu��ύX���邽�߂̕⊮���
    std::unique_ptr<AbyssEngine::FadeSystem> socketFade_;
};


