#include "VitesseState.h"
#include "Vitesse.h"
#include "Animator.h"
#include "Easing.h"
#include "Actor.h"
#include "Engine.h"
#include "Input.h"

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

}

void VitesseState::GroundMove::Finalize()
{
    owner_->ChangeAnimationState(Vitesse::AnimationState::Default);
}

void VitesseState::FlyMove::Initialize()
{
    //�A�j���[�V�����ݒ�
    //owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimState::Fly_Move));
    owner_->ChangeAnimationState(Vitesse::AnimationState::Fly_Move);

    //�󒆈ړ�
}

void VitesseState::FlyMove::Update()
{
    owner_->UpdateInputMove();
    owner_->ThrusterInfluenceVelocity();

    //�e�X�e�[�g�ɑJ��
    if (owner_->GetOnGround())
    {
        //���n���Ă���Ȃ璅�n�X�e�[�g��
        
        owner_->ChangeState(Vitesse::ActionState::Landing);
    }
}

void VitesseState::FlyMove::Finalize()
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
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Fly_Up));
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
    if (owner_->GetAnimator()->GetNextAnimClip() != static_cast<int>(Vitesse::AnimationIndex::Fly_Move)
        && owner_->GetAnimator()->GetCurrentAnimClip() == static_cast<int>(Vitesse::AnimationIndex::Fly_Up)
        && timer_ > requidTime_ / 4.0f)
    {
        //owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Fly_Move),requidTime_ / 4.0f);
        owner_->ChangeAnimationState(Vitesse::AnimationState::Fly_Move);
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
