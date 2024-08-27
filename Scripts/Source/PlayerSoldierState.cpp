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

    //�W�����v���\��
    owner_->SetCanJump(true);
}

void SoldierState::Move::Update()
{
    //�_�b�V������
    owner_->DashDecision();

    //�ړ�
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
    timer_ = 0.0f;

    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Aim));

    //�����U��������I�t��
    owner_->SetEnableAutoTurn(false);

    //�J��������点��
    owner_->GetCamera()->Zoom(zoomParam);

    //�W�����v��s��
    owner_->SetCanJump(false);

    //���s���x��x������
    owner_->SetMaxHorizontalSpeed(Max_Move_Speed);

}

void SoldierState::Aim::Update()
{
    //���ԍX�V
    timer_ += Time::deltaTime_;

    //����̈ʒu��ύX
    owner_->ChangeSocketTransformLinear(0.1f,
        owner_->Weapon_Offset_Aim.pos,
        owner_->Weapon_Offset_Aim.rot
    );

    //�ړ�
    owner_->InputMove();

    //�ˌ�
    if (Input::GameSupport::GetShotButton())
    {
        owner_->GunShot();
    }

    owner_->TurnY(owner_->GetCamera()->GetTransform()->GetForward(),30.0f);

    //�G�C���{�^����������Ă��Ȃ����A���Ԃ����΂炭�o���Ă����Ȃ�J��
    if (!Input::GameSupport::GetAimButton() && timer_ > Minimum_Time)
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

    //�W�����v���\��
    owner_->SetCanJump(true);
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

void SoldierState::Dodge::Initialize()
{
    //����A�j���[�V�����Đ�
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Fwd));

    //���[�g���[�V�����ɂ��ړ�����������
    owner_->GetAnimator()->SetEnableRootMotion(true);
}
