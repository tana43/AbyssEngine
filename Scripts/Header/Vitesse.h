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

    //���x�ɂ���ĉe�������X���X�^�[
    void ThrusterInfluenceVelocity();
    //�X���X�^�[�����ׂĒ�~
    void ThrusterAllStop();

public:
    //�s���X�e�[�g
    enum class ActionState
    {
        GMove,
        FMove,
        TakeOff,
        Landing,
        Boarding,
    };

    //�A�j���[�V����
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
        Board_Standby, //��荞�ݎp����
        Board_Complete,//��荞�ݎp�����痧���p����
        Run_Move,
        Fly_Move1D,
        Fly_Move2D,
        Fly_Move,
    };
    //�A�j���[�V�����X�e�[�g�}�V�[��Enum
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
    AbyssEngine::AnimBlendSpaceFlyMove* flyMoveAnimation_;//�󒆈ړ�

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
};

