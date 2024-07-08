#include "VitesseState.h"
#include "Vitesse.h"
#include "Animator.h"
#include "Easing.h"
#include "Actor.h"
#include "Engine.h"
#include "Input.h"

using namespace AbyssEngine;

void VitesseState::GMoveState::Initialize()
{
    //�A�j���[�V�����ݒ�
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimState::Run_Move));
}

void VitesseState::GMoveState::Update()
{
    //�W�����v�{�^���������ꂽ�ۂɗ����X�e�[�g��
    if (Input::GameSupport::GetJumpButton())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Vitesse::ActionState::TakeOff));
    }
}

void VitesseState::GMoveState::Finalize()
{
}

void VitesseState::FMoveState::Initialize()
{
    //�A�j���[�V�����ݒ�
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimState::Fly_Move));

    //�󒆈ړ�
}

void VitesseState::FMoveState::Update()
{
    owner_->ThrusterInfluenceVelocity();
}

void VitesseState::FMoveState::Finalize()
{
    owner_->ThrusterAllStop();
}

void VitesseState::LandingState::Initialize()
{
    //�A�j���[�V�����ݒ�
    //owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimState::));
}

void VitesseState::LandingState::Update()
{
}

void VitesseState::LandingState::Finalize()
{
}


void VitesseState::TakeOffState::Initialize()
{
    //�A�j���[�V�����ݒ�
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimState::Fly_Up));
    startPosition_ = owner_->GetTransform()->GetPosition().y;
    timer_ = 0;

    //�t���C�g���[�h�ֈڍs
    owner_->ToFlightMode();

    //�����͏����Ă���
    owner_->SetVelocityY(0.0f);

    //�ڕW���x��ݒ� 
    goalAltitude_ = startPosition_ + altitude_;
}

void VitesseState::TakeOffState::Update()
{
    //�A�j���[�V�������J�ڂ������Ă���A������x�̎��Ԃ��o�߂��Ă���Ȃ�󒆈ړ����[�V������
    if (owner_->GetAnimator()->GetNextAnimClip() != static_cast<int>(Vitesse::AnimState::Fly_Move)
        && owner_->GetAnimator()->GetCurrentAnimClip() == static_cast<int>(Vitesse::AnimState::Fly_Up)
        && timer_ > requidTime_ / 4.0f)
    {
        owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimState::Fly_Move),requidTime_ / 4.0f);
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

void VitesseState::TakeOffState::Finalize()
{
}
