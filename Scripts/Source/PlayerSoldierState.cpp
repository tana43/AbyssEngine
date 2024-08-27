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

    //���
    if (Input::GameSupport::GetDashButton())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Dodge));
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
    //���X�e�B�b�N
    Vector2 ax = Input::GameSupport::GetMoveVector();
    if (ax.LengthSquared() == 0)
    {
        Vector3 f = owner_->GetTransform()->GetForward();
        f.Normalize();
        ax = { f.x,f.z };
    }
    Vector3 moveVec = owner_->GetCamera()->ConvertTo2DVectorFromCamera(ax);
    Direction dir = DirectionJudge(moveVec);

    //����A�j���[�V�����Đ�
    switch (dir)
    {
    case SoldierState::Dodge::Direction::Back:
        owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Back));
        break;
    case SoldierState::Dodge::Direction::Forward:
        owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Fwd));
        break;
    case SoldierState::Dodge::Direction::Right:
        owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Right));
        break;
    case SoldierState::Dodge::Direction::Left:
        owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Left));
        break;
    }

    timer_ = 0;

    secondDodge_ = false;
}

void SoldierState::Dodge::Update()
{
    if (!secondDodge_)
    {
        if (timer_ > Cancel_Time)
        {
            if (Input::GameSupport::GetDodgeButton())
            {
                Vector2 ax = Input::GameSupport::GetMoveVector();
                if (ax.LengthSquared() == 0)
                {
                    Vector3 f = owner_->GetTransform()->GetForward();
                    f.Normalize();
                    ax = { f.x,f.z };
                }
                Vector3 moveVec = owner_->GetCamera()->ConvertTo2DVectorFromCamera(ax);
                Direction dir = DirectionJudge(moveVec);

                //����A�j���[�V�����Đ�
                float transTime = 0.3f;//�J�ڎ���
                switch (dir)
                {
                case SoldierState::Dodge::Direction::Back:
                    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Cartwheel_Back), transTime);
                    break;
                case SoldierState::Dodge::Direction::Forward:
                    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Cartwheel_Forward), transTime);
                    break;
                case SoldierState::Dodge::Direction::Right:
                    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::NoHandSpin_Right), transTime);
                    break;
                case SoldierState::Dodge::Direction::Left:
                    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::NoHandSpin_Left), transTime);
                    break;
                }

                //�^�C�}�[���Z�b�g
                //timer_ = 0;

                secondDodge_ = true;
            }
        }
    }

    //�X�e�[�g�̐؂�ւ�
    if (owner_->GetAnimator()->GetAnimationFinished())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Move));
    }

    //�o�ߎ���
    timer_ += Time::deltaTime_;
}

void SoldierState::Dodge::Finalize()
{
}

SoldierState::Dodge::Direction SoldierState::Dodge::DirectionJudge(const Vector3& moveVec)
{
    float dot = moveVec.Dot(owner_->GetTransform()->GetForward());
    float degree = DirectX::XMConvertToDegrees(acosf(dot));
    
    //�������Z�o
    Direction dir = Direction::Forward;
    if (degree < 45.0f)
    {
        dir = Direction::Forward;
    }
    else if(degree < 135.0f)
    {
        //���E����
        if (moveVec.x > 0)
        {
            dir = Direction::Right;
        }
        else
        {
            dir = Direction::Left;
        }
    }
    else
    {
        dir = Direction::Back;
    }

    return dir;
}
