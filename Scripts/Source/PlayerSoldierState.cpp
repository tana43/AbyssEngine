#include "PlayerSoldierState.h"
#include "PlayerSoldier.h"
#include "Vitesse.h"
#include "Transform.h"
#include "Input.h"

#include "Engine.h"

using namespace AbyssEngine;

void SoldierState::Move::Initialize()
{
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Move));
}

void SoldierState::Move::Update()
{
    owner_->InputMove();

    //����̈ʒu��ύX
    owner_->ChangeSocketTransformLinear(0.1f,
        owner_->Weapon_Offset_Move.pos,
        owner_->Weapon_Offset_Move.rot
    );

    //���B�e�X�֓���
    if (Input::GameSupport::GetBoardingButton())
    {
        owner_->BoardingTheVitesse();
    }

    //�G�C���{�^����������Ă���Ȃ�J��
    if (Input::GameSupport::GetAimButton())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Aim));
    }
}

void SoldierState::Move::Finalize()
{

}

void SoldierState::Aim::Initialize()
{
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Aim));

    //�����U��������I�t��
    owner_->SetEnableAutoTurn(false);

    //�J��������点��
    owner_->GetCamera()->Zoom(zoomParam);
}

void SoldierState::Aim::Update()
{
    //����̈ʒu��ύX
    owner_->ChangeSocketTransformLinear(0.1f,
        owner_->Weapon_Offset_Aim.pos,
        owner_->Weapon_Offset_Aim.rot
    );

    //�ˌ�
    if (Input::GameSupport::GetShotButton())
    {
        owner_->GunShot();
    }

    owner_->TurnY(owner_->GetCamera()->GetTransform()->GetForward(),30.0f);

    //�G�C���{�^����������Ă��Ȃ��Ȃ�J��
    if (!Input::GameSupport::GetAimButton())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Move));
    }
}

void SoldierState::Aim::Finalize()
{
    //�����U��������I����
    owner_->SetEnableAutoTurn(true);

    //�J������߂�
    owner_->GetCamera()->ZoomReset(zoomParam.time_);
}

void SoldierState::Jump::Initialize()
{
    //�W�����v�A�j���[�V�����Đ�
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Jump));

    landTimer_ = 0.0f;
}

void SoldierState::Jump::Update()
{
    //�ړ�
    owner_->InputMove();

    //�W�����v���[�V�����Đ��I�����Ƀ��[�v���[�V�����֑J��
    if (owner_->GetAnimator()->GetAnimationFinished())
    {
        owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Fall_Loop));
    }

    //���n
    if (owner_->GetOnGround())
    {
        if (landTimer_ == 0)
        {
            //���n��A�j���[�V�����Đ�
            owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Land));
        }

        //�^�C�}�[�X�V
        landTimer_ += Time::deltaTime_;
    }

    //���n���Ďw��b���o���Ă���Ȃ�ړ��X�e�[�g�ɑJ��
    if (landTimer_ > 0.3f)
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Move));
    }
}

void SoldierState::Jump::Finalize()
{
    

}
