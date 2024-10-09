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

    //���x�ɂ���ĉe�������X���X�^�[
    void ThrusterInfluenceVelocity();
    //�X���X�^�[�����ׂĒ�~
    void ThrusterAllStop();

    

    //����s��
    void Dodge(AbyssEngine::Vector3 direction/*����s��*/);

    //�X�e�b�v�ړ�(�������e�͉���Ɠ���)
    void StepMove(AbyssEngine::Vector3 moveDirection, float speed);

    //�U�����肪�q�b�g����
    void OnCollision(const std::shared_ptr<AbyssEngine::Collider>& hitCollider, 
        AbyssEngine::Collision::IntersectionResult result)override;

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
        MeleeAtkDash,
        MeleeAtk,
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
        Slash_Dash_Loop_R,
        Slash_Dash_Loop_L,
        Slash_N_1,
        Slash_N_2,
        Slash_N_2_End,
        Slash_N_3,
        Slash_N_3_End,
        Slash_R_1,

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

    //�^�[�Q�b�g�܂ł̃x�N�g�����Z�o
    //�^�[�Q�b�g�����Ȃ��ꍇ�͌��Ă��������Ԃ�
    AbyssEngine::Vector3 ToTarget();

    void ChangeActionState(const ActionState& state);
    void ChangeAnimationState(const AnimationState& state);
    

    //�p�C���b�g��荞��
    void GetOnBoardPilot(const std::shared_ptr<Soldier>& pilot);

    //�ړ����͂̔��f
    void UpdateInputMove()override;

    //�n�ヂ�[�h�ֈڍs
    void ToGroundMode()override;

    //�^�[�Q�b�g�̃R���C�_�[��⑫����
    void TargetAcquisition();

    //�A�j���[�V�����Đ�
    void PlayAnimation(AnimationIndex index,float transTime = 0.1f,float startTime = 0.0f);

    //���W�A���u���[���^�[�Q�b�g�����鏊�𒆐S�Ƃ��ĉ��o������
    void RadialBlurFromTarget();

private:
    void CameraRollUpdate();

    //�X���X�^�[�X�V����
    void ThrusterUpdate();

    //�p�C���b�g�����悵�Ă��邩�𔻒肷��Ƃ�
    void PilotUpdate();

    //�㏸�̓��͂𔽉f������
    void RiseInputUpdate();

    //�R���C�_�[�ݒ�
    void ColliderInitialize();

    //�A�^�b�J�[�ݒ�(�R���C�_�[��ݒ肵���オ��������)
    void AttackerInitialize();
    
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
    
    //�E�蕐��
    std::shared_ptr<AbyssEngine::StaticMesh> rightWeaponModel_;
    //���蕐��
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

    //���b�N�I�����Ă��邩
    bool activeLockon_ = false;
    //�⑫�A�������̓��b�N�I���\�ɂȂ�͈�
    float lockRadius_ = 200.0f;

    //�ڕW�ƂȂ�A�N�^�[
    std::weak_ptr<AbyssEngine::Actor> lockonTarget_;

    //���b�N�I�����Ă���^�[�Q�b�g���ύX���ꂽ�Ƃ���true
    bool changeLockonTarget_ = false;

    //�ߐڍU��������ۂɓG�ɋߊ�鑬�x
    float meleeAtkDashSpeed_ = 70.0f;
    float meleeAtkDashMaxSpeed_ = 70.0f;

    //�ߐڍU�����\�ɂȂ�͈�
    float meleeAtkRange_ = 25.0f;

    //�ߐڍU�����̑��x
    float meleeAtkSpeed_ = 10.0f;

    //�A�^�b�J�[�V�X�e��
    std::shared_ptr<AbyssEngine::AttackerSystem> attackerSystem_;

    //�A�^�b�N�R���C�_�[
    std::vector<std::shared_ptr<AbyssEngine::AttackCollider>> lWeaponAtkColliderList_;
    std::vector<std::shared_ptr<AbyssEngine::AttackCollider>> rWeaponAtkColliderList_;
};

