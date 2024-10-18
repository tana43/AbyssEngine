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

void SoldierState::Move::Update(float deltaTime)
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

void SoldierState::Aim::Update(float deltaTime)
{
    //���ԍX�V
    timer_ += deltaTime;

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

void SoldierState::Jump::Update(float deltaTime)
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
        landTimer_ += deltaTime;
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
    PlayDodgeAnimation(dir, false);

    timer_ = 0;

    secondDodge_ = false;
}

void SoldierState::Dodge::Update(float deltaTime)
{
    if (!secondDodge_)
    {
        //���[�����O���͎��R�ɐ��񂳂���
        bool canTurn = true;

        Vector2 ax = Input::GameSupport::GetMoveVector();
        if (ax.LengthSquared() == 0)
        {
            Vector3 f = owner_->GetTransform()->GetForward();
            f.Normalize();
            ax = { f.x,f.z };

            //���͒l���Ȃ��Ƃ��͐��񂵂Ȃ�
            canTurn = false;
        }
        Vector3 moveVec = owner_->GetCamera()->ConvertTo2DVectorFromCamera(ax);

        //���݂̍Đ����[�V�����ɍ��킹�Đ���
        if (canTurn)
        {
            Vector3 turnVec = moveVec;

            //TODO:�������␳�@������ƕʂ̂��Ƃ����Ă�����
            //�������ɍ��킹�Đ�������𒲐�
            
            //��]�����𒲐����邽�߂̕���
            Vector3 forward = {0,0,1};
            Vector3 right = {1,0,0};

            switch (currentDirection_)
            {
            case SoldierState::Dodge::Direction::Back:
                //foward = ��
                forward = { 0, 0,-1 };
                right   = {-1, 0, 0 };
                break;
            case SoldierState::Dodge::Direction::Forward:
                //foward = ���@���̂܂�

                break;
            case SoldierState::Dodge::Direction::Right:
                //foward = ��
                forward = { 1, 0, 0 };
                right   = { 0, 0,-1 };
                break;
            case SoldierState::Dodge::Direction::Left:
                //foward = ��
                forward = {-1, 0, 0 };
                right   = { 0, 0, 1 };
                break;
            default:
                break;
            }

            turnVec = {
                    forward.x * moveVec.z + right.x * moveVec.x,
                    0,
                    forward.z * moveVec.z + right.z * moveVec.x };

            owner_->TurnY(turnVec, 200.0f * deltaTime, true);
        }

        //���]����ւ̑J��
        if (timer_ > Dodge_Cancel_Time)
        {
            if (Input::GameSupport::GetDodgeButton())
            {
                
                Direction dir = DirectionJudge(moveVec);

                PlayDodgeAnimation(dir,true);

                //�^�C�}�[���Z�b�g
                timer_ = 0;

                secondDodge_ = true;
            }
        }
    }

    //����̈ʒu��ύX
    owner_->ChangeSocketTransformLinear(0.1f,
        owner_->Weapon_Offset_Move.pos,
        owner_->Weapon_Offset_Move.rot
    );

    //�X�e�[�g�̐؂�ւ�
    if (owner_->GetAnimator()->GetAnimationFinished())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Move));
    }

    //�L�����Z���s��
    //���݃L�����Z���\��
    bool otherCancel = false;
    bool moveCancel = false;
    if (secondDodge_)
    {
        if (timer_ > Other_Cancel_Time[1])otherCancel = true;
        if (timer_ > Move_Cancel_Time[1])moveCancel = true;
    }
    else
    {
        if (timer_ > Other_Cancel_Time[0])otherCancel = true;
        if (timer_ > Move_Cancel_Time[0])moveCancel = true;
    }



    if (otherCancel)
    {
        //�G�C���{�^����������Ă���Ȃ�J��
        if (Input::GameSupport::GetAimButton())
        {
            owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Aim));
        }
    }

    if (moveCancel)
    {
        //�ړ��X�e�[�g��
        if (Input::GameSupport::GetMoveVector().LengthSquared() > 0.1f)
        {
            owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Move));
        }
    }

    //�o�ߎ���
    timer_ += deltaTime;
}

void SoldierState::Dodge::Finalize()
{
}

SoldierState::Dodge::Direction SoldierState::Dodge::DirectionJudge(const Vector3& moveVec)
{
    const Vector3& forward = owner_->GetTransform()->GetForward();
    float dot = moveVec.Dot(forward);
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
        float crossY = moveVec.z * forward.x - moveVec.x * forward.z;
        if (crossY > 0)
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

void SoldierState::Dodge::PlayDodgeAnimation(Direction dir, bool second)
{
    //����A�j���[�V�����Đ�
    float transTime = 0.1f;//�J�ڎ���
    float startTime = 0.15f;//���b�ڂ���̃��[�V�������Đ������邩
    switch (dir)
    {
    case SoldierState::Dodge::Direction::Back:
        if(second)owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Cartwheel_Back), &transTime, startTime);
        else owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Back));
        break;
    case SoldierState::Dodge::Direction::Forward:
        if (second)owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Cartwheel_Forward), &transTime, startTime);
        else owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Fwd));
        break;
    case SoldierState::Dodge::Direction::Right:
        if (second)owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::NoHandSpin_Right), &transTime, startTime);
        else owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Right));
        break;
    case SoldierState::Dodge::Direction::Left:
        if (second)owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::NoHandSpin_Left), &transTime, startTime);
        else owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Left));
        break;
    }
    currentDirection_ = dir;
}

void SoldierState::Die::Initialize()
{
}

void SoldierState::Die::Update(float deltaTime)
{
}

void SoldierState::Die::Finalize()
{
}
