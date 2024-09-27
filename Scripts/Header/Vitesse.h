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

    //���x�ɂ���ĉe�������X���X�^�[
    void ThrusterInfluenceVelocity();
    //�X���X�^�[�����ׂĒ�~
    void ThrusterAllStop();

    //����s��
    void Dodge(AbyssEngine::Vector3 direction/*����s��*/);

public:
    //�s���X�e�[�g
    enum class ActionState
    {
        GMove,
        FMove,
        TakeOff,
        Landing,
        Boarding,
        HighSpeedFlight,
    };

    //�A�j���[�V����
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
        Board_Standby, //��荞�ݎp����
        Board_Complete,//��荞�ݎp�����痧���p����
        HighSpeedFlight_F,
        HighSpeedFlight_R,
        HighSpeedFlight_L,
        HighSpeedFlight_B,

        Dodge_FR,
        Dodge_FL,

        //�n��ړ����ɗ��������ۂ̋���
        Ground_Fall,

        //�ߐڍU��
        Slash_Dash_Start,
        Slash_Dash_Loop,
        Slash_N_1,

        //�u�����h���[�V����
        Run_Move,
        Flight_Move1D,
        Flight_Move2D,
        Flight_Move,
        HighSpeedFlight_Move2D,
        HighSpeedFlight_Move,

        
    };
    //�A�j���[�V�����X�e�[�g�}�V�[��Enum
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

    const AbyssEngine::Vector3& GetMoveVec() const { return moveVec_; }
    void SetMoveVec(const AbyssEngine::Vector3& vec) { moveVec_ = vec; }

    const AbyssEngine::Vector3& GetMoveDirection() const { return moveDirection_; }
    void SetMoveDirection(const AbyssEngine::Vector3& vec) { moveDirection_ = vec; }

    const bool& GetCanBoarding() const { return canBoarding_; }
    void SetCanBoarding(const bool& can) { canBoarding_ = can; }

    const float& GetDodgeMaxSpeed() const { return dodgeMaxSpeed_; }
    const float& GetHighSpeedFlightMaxSpeed() const { return highSpeedFlightMaxSpeed_; }

    const std::shared_ptr<AbyssEngine::Camera>& GetCamera() const { return camera_; }

    const float& GetDefaultCameraLagSpeed() const { return defaultCameraLagSpeed_; }

    void ChangeActionState(const ActionState& state);
    void ChangeAnimationState(const AnimationState& state);
    

    //�p�C���b�g��荞��
    void GetOnBoardPilot(const std::shared_ptr<Soldier>& pilot);

    //�ړ����͂̔��f
    void UpdateInputMove()override;

    //�n�ヂ�[�h�ֈڍs
    void ToGroundMode()override;

private:
    void CameraRollUpdate();

    //�X���X�^�[�X�V����
    void ThrusterUpdate();

private:
    std::shared_ptr<AbyssEngine::Camera> camera_;

#if 1
    AbyssEngine::AnimBlendSpace2D* groundMoveAnimation_;//����ړ�
#else
    AbyssEngine::AnimBlendSpace1D* runMoveAnimation_;//����ړ�
#endif // 0
    //AbyssEngine::AnimBlendSpace2D* flyMoveAnimation_;//�󒆈ړ�
    AbyssEngine::AnimBlendSpaceFlyMove* flightAnimation_;//�󒆈ړ�
    AbyssEngine::AnimBlendSpaceFlyMove* highSpeedFlightAnimation_;//�����󒆈ړ�

    std::shared_ptr<AbyssEngine::StateMachine<State<Vitesse>>> stateMachine_;
    std::shared_ptr<AbyssEngine::StateMachine<State<AbyssEngine::Animator>>> animStateMachine_;

    //�K�v�Ȋ���̃X���X�^�[
    std::shared_ptr<ThrusterEffect> thrusters_[static_cast<int>(VitesseConstants::Thruster::Location::Installed_Units)];
    //std::shared_ptr<ThrusterEffect> thruster_;

    float cameraRollSpeed_ = 90.0f;
    
    //�������Ă�������Ƒ��x�������Ă����������
    AbyssEngine::Vector3 moveDirection_ = { 0,0,0 };

    //�X���X�^�[�𕬎˂����邩
    bool activeThruster_ = true;

    //�p�C���b�g(�v���C���[)
    std::weak_ptr<Soldier> pilot_;

    //��荞�݉\�ȏ�Ԃ�
    bool canBoarding_ = false;

    //����s��
    AbyssEngine::Vector3 dodgeDirection_ = {0,0,0};
    //��𑬓x
    float dodgeSpeed_ = 100.0f;
    float dodgeMaxSpeed_ = 100.0f;

    //������s�ړ����x
    float highSpeedFlightMaxSpeed_ = 50.0f;

    //�J�����̃f�t�H���g�l
    float defaultCameraLagSpeed_ = 0.0f;
    
    //����
    std::shared_ptr<AbyssEngine::StaticMesh> weaponModel_;

    struct SocketOffset
    {
        AbyssEngine::Vector3 pos;
        AbyssEngine::Vector3 rot;
    };
    const SocketOffset Weapon_Offset =
    {
        {-42.650,-5.65f,-20.8f},
        {-21.2,180.85f,93.95f},
    };
};

