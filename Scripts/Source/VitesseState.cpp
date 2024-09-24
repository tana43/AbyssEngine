#include "VitesseState.h"
#include "Vitesse.h"
#include "Animator.h"
#include "Easing.h"
#include "Actor.h"
#include "Engine.h"
#include "Input.h"
#include "RenderManager.h"

using namespace AbyssEngine;

void VitesseState::GroundMove::Initialize()
{
    //�A�j���[�V�����ݒ�
    owner_->ChangeAnimationState(Vitesse::AnimationState::Ground_Move);
}

void VitesseState::GroundMove::Update()
{
    owner_->UpdateInputMove();

    //�W�����v�{�^���������ꂽ�ۂɗ����X�e�[�g��
    if (Input::GameSupport::GetJumpButton())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Vitesse::ActionState::TakeOff));
    }

    //�N������Ă��Ȃ��Ȃ��荞�݃X�e�[�g��
    if (!owner_->GetPilot().lock())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Vitesse::ActionState::Boarding));
    }

    //�_�b�V���{�^����������Ă���Ȃ獂���X�e�[�g��
    if (Input::GameSupport::GetDashButton())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Vitesse::ActionState::HighSpeedFlight));
    }
}

void VitesseState::GroundMove::Finalize()
{
    owner_->ChangeAnimationState(Vitesse::AnimationState::Default);
}

void VitesseState::Flight::Initialize()
{
    //�A�j���[�V�����ݒ�
    //owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimState::Fly_Move));
    owner_->ChangeAnimationState(Vitesse::AnimationState::Flight_Move);

    //�󒆈ړ�
}

void VitesseState::Flight::Update()
{
    owner_->UpdateInputMove();
    owner_->ThrusterInfluenceVelocity();

    //�e�X�e�[�g�ɑJ��
    
    //���n���Ă���Ȃ璅�n�X�e�[�g��
    if (owner_->GetOnGround())
    {    
        owner_->ChangeState(Vitesse::ActionState::Landing);
    }

    //�_�b�V���{�^����������Ă���Ȃ獂���X�e�[�g��
    if (Input::GameSupport::GetDashButton())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Vitesse::ActionState::HighSpeedFlight));
    }
}

void VitesseState::Flight::Finalize()
{
    //�A�j���X�e�[�g���f�t�H���g��
    owner_->ChangeAnimationState(Vitesse::AnimationState::Default);

    //�X���X�^�[�S��~
    owner_->ThrusterAllStop();
}

void VitesseState::Landing::Initialize()
{
    //�A�j���[�V�����ݒ�
    //owner_->ChangeAnimationState(Vitesse::AnimationState::Fly_Move);
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Landing));

    //���ヂ�[�h�ֈڍs
    owner_->ToGroundMode();

    //���������邽�߂Ɉړ��������O��
    owner_->SetMoveVec(Vector3::Zero);

    //�����͂��L���b�V���A����
    cachedDeceleration_ = owner_->GetDeceleration();
    owner_->SetDeceleration(deceleration_);
}

void VitesseState::Landing::Update()
{
    //�A�j���[�V�������I������GroundMove��
    if (owner_->GetAnimator()->GetAnimationFinished())
    {

        owner_->ChangeState(Vitesse::ActionState::GMove);
    }
}

void VitesseState::Landing::Finalize()
{
    //�����͂����ɖ߂�
    owner_->SetDeceleration(cachedDeceleration_);
}


void VitesseState::TakeOff::Initialize()
{
    //�A�j���[�V�����ݒ�
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Flight_Up));
    startPosition_ = owner_->GetTransform()->GetPosition().y;
    timer_ = 0;

    //�t���C�g���[�h�ֈڍs
    owner_->ToFlightMode();

    //�����͏����Ă���
    owner_->SetVelocityY(0.0f);

    //���n�t���O�����낵�Ă���
    owner_->SetOnGround(false);

    //�ڕW���x��ݒ� 
    goalAltitude_ = startPosition_ + altitude_;
}

void VitesseState::TakeOff::Update()
{
    //�A�j���[�V�������J�ڂ������Ă���A������x�̎��Ԃ��o�߂��Ă���Ȃ�󒆈ړ����[�V������
    if (owner_->GetAnimator()->GetNextAnimClip() != static_cast<int>(Vitesse::AnimationIndex::Flight_Move)
        && owner_->GetAnimator()->GetCurrentAnimClip() == static_cast<int>(Vitesse::AnimationIndex::Flight_Up)
        && timer_ > requidTime_ / 4.0f)
    {
        //owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Fly_Move),requidTime_ / 4.0f);
        owner_->ChangeAnimationState(Vitesse::AnimationState::Flight_Move);
    }

    //�C�[�W���O���g����Y���W����ɓ�����
    float positionY = Easing::OutCubic(timer_, requidTime_, goalAltitude_,startPosition_);

    owner_->GetTransform()->SetPositionY(positionY);

    timer_ += Time::deltaTime_;


    //�󒆂փX�e�[�g�J��
    if (timer_  > requidTime_)
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Vitesse::ActionState::FMove));
    }
}

void VitesseState::TakeOff::Finalize()
{
}

void VitesseState::Boarding::Initialize()
{
    //�A�j���[�V�����ݒ�
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Board_Standby));

    //��荞�݉\�ȏ�Ԃ�
    owner_->SetCanBoarding(true);

    board_ = false;
}

void VitesseState::Boarding::Update()
{
    //��荞��ł��Ȃ��Ƃ�
    //�A�j���[�V�������I�����Ă���A�v���C���[����荞��ł���Ȃ�ړ��X�e�[�g�֑J��
    if (!board_)
    {
        if (owner_->GetAnimator()->GetAnimationFinished())
        {
            if (owner_->GetPilot().lock())
            {
                owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Board_Complete));
                board_ = true;
            }
        }
    }
    //��荞�񂾂Ȃ�A�A�j���[�V�����̏I���Ɠ����ɃX�e�[�g��J��
    else
    {
        if (owner_->GetAnimator()->GetAnimationFinished())
        {
            owner_->GetStateMachine()->ChangeState(static_cast<int>(Vitesse::ActionState::GMove));
        }
    }
}

void VitesseState::Boarding::Finalize()
{
    //�A�j���[�V�����ݒ�
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Board_Complete));

    //��荞�ݕs��
    owner_->SetCanBoarding(false);
}

void VitesseState::HighSpeedFlight::Initialize()
{
    rollingDodge_ = false;

    //���(���͒l�������ꍇ�A�O�����։��)
    auto inputVec = Input::GameSupport::GetMoveVector();
    auto moveVec = owner_->GetCamera()->ConvertTo3DVectorFromCamera(inputVec);
    if (moveVec.LengthSquared() < 0.1f)moveVec = owner_->GetTransform()->GetForward();
    owner_->Dodge(moveVec);
    owner_->SetMoveVec(moveVec);

    //�A�j���[�V�����ݒ�
    //�΂ߓ��͂̂Ƃ��́A��p���[�V������
    inputVec.Normalize();
    const float radius = acosf(inputVec.Dot(Vector2(0, 1)));
    //�������l�ݒ�
    const float rollingDodgeArea[2] = { DirectX::XMConvertToRadians(30),DirectX::XMConvertToRadians(60) };
    const float rollingDodgeAreaBack[2] = { DirectX::XMConvertToRadians(120),DirectX::XMConvertToRadians(150) };
    if ((radius > rollingDodgeArea[0] && radius < rollingDodgeArea[1]) || 
        (radius > rollingDodgeAreaBack[0] && radius < rollingDodgeAreaBack[1]))
    {
        if (inputVec.x > 0)
        {
            owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Dodge_FR));
        }
        else
        {
            owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Dodge_FR));
        }
        rollingDodge_ = true;
    }
    else
    {
        owner_->ChangeAnimationState(Vitesse::AnimationState::HighSpeedFlight);
    }

    //��x���������Ă��獂���ړ���
    //�t���C�g���[�h�ֈڍs
    owner_->ToFlightMode();
    owner_->SetOnGround(false);

    
    //�ő呬�x��ύX
    owner_->SetMaxHorizontalSpeed(owner_->GetDodgeMaxSpeed());
    owner_->SetMaxVerticalSpeed(owner_->GetDodgeMaxSpeed());

    //���W�A���u���[�ݒ�
    auto& postEffect = Engine::renderManager_->GetBufferEffects().data_;
    postEffect.radialBlurStrength_ = 0.3f;
    postEffect.radialBlurSampleCount_ = 3;

    //�J�����̃Y�[����ύX
    auto& camera = owner_->GetCamera();
    Camera::ZoomParam zoomParam;
    zoomParam.armLength_ = dodgeCameraArmLength_;
    zoomParam.time_ = 0.05f;
    zoomParam.socketOffset_ = camera->GetSocketOffset();
    zoomParam.targetOffset_ = camera->GetTargetOffset();
    camera->Zoom(zoomParam);
    camera->SetCameraLagSpeed(cameraLagSpeed);

    timer_ = 0.0f;
}

void VitesseState::HighSpeedFlight::Update()
{
    //�X���X�^�[����
    owner_->ThrusterInfluenceVelocity();

    //�ړ������̐ݒ�
    owner_->UpdateInputMove();
    //�������͂��Ȃ��Ƃ��͑O�����֓�����
    if (owner_->GetMoveVec().LengthSquared() < 0.1f)
    {
        owner_->SetMoveVec(owner_->GetCamera()->ConvertTo3DVectorFromCamera(Vector2(0,1)));
    }

    //���[�����O����I������
    if (rollingDodge_)
    {
        if (owner_->GetAnimator()->GetAnimationFinished())
        {
            rollingDodge_ = false;

            //�ʏ����ړ���
            //��]���郂�[�V�����ł���s����A��U�����ړ��̃��[�V���������ޕK�v������
            owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::HighSpeedFlight_F),0.0f);
            owner_->ChangeAnimationState(Vitesse::AnimationState::HighSpeedFlight);
        }
    }

    //�J�����A�[���𒲐�
    float blendWeight = fminf(timer_ / dodgeTime_,1.0f);
    float armLemgth = std::lerp(dodgeCameraArmLength_, highSpeedCameraArmLength_, blendWeight);
    owner_->GetCamera()->SetArmLength(armLemgth);

    //����I��
    if (timer_ > dodgeTime_ && !rollingDodge_)
    {
        //�ő呬�x�������ړ����̒l�ɕύX
        owner_->SetMaxHorizontalSpeed(owner_->GetHighSpeedFlightMaxSpeed());
        owner_->SetMaxVerticalSpeed(owner_->GetHighSpeedFlightMaxSpeed());
    }

    //���W�A���u���[�ݒ�
    auto& postEffect = Engine::renderManager_->GetBufferEffects().data_;
    const float blurStrength = 0.07f;
    if (postEffect.radialBlurStrength_ > blurStrength)
    {
        postEffect.radialBlurStrength_ -= Time::deltaTime_;
    }
    else
    {
        postEffect.radialBlurStrength_ = blurStrength;
    }

    timer_ += Time::deltaTime_;

    //�e�X�e�[�g�ɑJ��

    //��𒆂͒ʏ�ړ��ֈڍs���Ȃ�
    if (timer_ > dodgeTime_ && !rollingDodge_)
    {
        //�_�b�V���{�^����������Ă��Ȃ��Ȃ�ʏ��s�X�e�[�g��
        if (!Input::GameSupport::GetDashButton())
        {
            owner_->ChangeState(Vitesse::ActionState::FMove);
            return;
        }
    }

    //���n���Ă���Ȃ璅�n�X�e�[�g��
    /*if (owner_->GetOnGround())
    {
        owner_->ChangeState(Vitesse::ActionState::Landing);
        return;
    }*/
}

void VitesseState::HighSpeedFlight::Finalize()
{
    //�A�j���[�V�����X�e�[�g���f�t�H���g��
    owner_->ChangeAnimationState(Vitesse::AnimationState::Default);

    //�ő呬�x�����ɖ߂�
    owner_->SetMaxHorizontalSpeed(owner_->GetDefaultMaxHorizontalSpeed());
    owner_->SetMaxVerticalSpeed(owner_->GetDefaultMaxVerticalSpeed());

    //���W�A���u���[������
    auto& postEffect = Engine::renderManager_->GetBufferEffects().data_;
    postEffect.radialBlurStrength_ = 0.0f;
    postEffect.radialBlurSampleCount_ = 1;

    //�Y�[�������Z�b�g
    owner_->GetCamera()->ZoomReset(1.5f);
    owner_->GetCamera()->SetCameraLagSpeed(owner_->GetDefaultCameraLagSpeed());

    if (owner_->GetAnimator()->GetCurrentAnimClip() == static_cast<int>(Vitesse::AnimationIndex::Dodge_FR) ||
        owner_->GetAnimator()->GetCurrentAnimClip() == static_cast<int>(Vitesse::AnimationIndex::Dodge_FL))
    {
        owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::HighSpeedFlight_F),0.0f);
    }
}
