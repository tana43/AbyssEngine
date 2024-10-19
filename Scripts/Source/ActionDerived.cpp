#include "ActionDerived.h"
#include "BaseEnemy.h"
#include "BotEnemy.h"
#include "Transform.h"
#include "Animator.h"
#include "Engine.h"
#include "BossMech.h"
#include "AttackerSystem.h"

using namespace AbyssEngine;

#pragma region �{�b�g�G�l�~�[

// �U���s��
ActionBase<BotEnemy>::State BotAttackAction::Run(float deltaTime)
{
	auto state = ActionBase::State::Run;

	switch (step)
	{
	case static_cast<int>(Step::Init)://������
		//�^�C�}�[���Z�b�g
		timer_ = 0;

		//�A�j���[�V�����Đ�
		owner_->GetAnimator()->PlayAnimation(static_cast<int>(BotEnemy::AnimState::Attack));

		//���̃X�e�b�v��
		step++;

		break;
	case static_cast<int>(Step::LockOn):

		//�^�[�Q�b�g�����b�N�I��
		owner_->LockOn();

		//�^�[�Q�b�g�𐳊m�Ƀ��b�N�I�����鎞�Ԃ��o�߂����玟�̃X�e�b�v��
		if (timer_ > owner_->GetLockOnTime())
		{
			//�^�C�}�[���Z�b�g
			timer_ = 0;

			step++;
		}

		//�^�C�}�[�X�V
		timer_ += deltaTime;

		break;
	case static_cast<int>(Step::LockIdle):

		//�ҋ@���Ԃ��o�߂����玟�̃X�e�b�v��
		if (timer_ > owner_->GetLockOnShotTime())
		{
			//�^�C�}�[���Z�b�g
			timer_ = 0;

			step++;
		}

		//�^�C�}�[�X�V
		timer_ += deltaTime;
		break;

	case static_cast<int>(Step::Shot):


		//�ˌ�
		owner_->Shot();

		//�����I�������ɗ]�C������������������.........�H

		step++;

		break;

	case static_cast<int>(Step::AnimFinishWait):

		if (owner_->GetAnimator()->GetAnimationFinished())
		{
			//�s������
			state = ActionBase::State::Complete;

			//�X�e�b�v���Z�b�g
			step = 0;
		}
		break;
	}


	return state;
}


ActionBase<BotEnemy>::State BotSideDodgeAction::Run(float deltaTime)
{
	switch (step)
	{
	case 0:

		//�R���R����]����A�j���[�V�����Đ�
		owner_->GetAnimator()->PlayAnimation(static_cast<int>(BotEnemy::AnimState::Rolling));

		//�����ō��E�ǂ���ɐi�ނ���ݒ�
		moveRight_ = rand() % 2;

		//������Ԑݒ�
		dodgeTime_ = 1.5 + (static_cast<float>(rand() % 10) / 10.0f);

		//���̃X�e�b�v��
		step++;

		break;

	case 1:

		//�^�[�Q�b�g�𒆐S�ɉ�]����悤�Ɉړ�
		owner_->SideMove(owner_->GetTargetActor()->GetTransform()->GetPosition(), moveRight_);

		//�����[�h���������Ă���Ȃ�I��
		//if (owner_->GetAnimator()->GetAnimationFinished())
		if (timer_ > dodgeTime_)
		{
			step = 0;

			timer_ = 0;

			//�s������
			return ActionBase::State::Complete;
		}

		timer_ += deltaTime;

		break;
	}


	return ActionBase::State::Run;
}

ActionBase<BotEnemy>::State BotIdleAction::Run(float deltaTime)
{
	switch (step)
	{
	case 0://������

		//�ҋ@�A�j���[�V�����Đ�
		owner_->GetAnimator()->PlayAnimation(static_cast<int>(BotEnemy::AnimState::Idle));

		//�^�C�}�[���Z�b�g
		timer_ = 0.0f;

		//���̃X�e�b�v��
		step++;

		break;

	case 1:
		//TODO : �G�ҋ@�s���@���łS�b�Ԃɐݒ肵�Ă�
		if (timer_ > 4.0f)
		{
			step = 0;

			//�s������
			return State::Complete;
		}

		//�^�C�}�[�X�V
		timer_ += deltaTime;
		break;
	}

	return State::Run;
}

//�p�j�s��
ActionBase<BotEnemy>::State BotWonderActioin::Run(float deltaTime)
{
	switch (step)
	{
	case 0:
		// �p�j���[�V�����ݒ�
		owner_->GetAnimator()->PlayAnimation(static_cast<int>(BotEnemy::AnimState::Walk));

		// �ڕW���W�ݒ�
		owner_->SetRandomTargetPosition();

		step++;
		break;
	case 1:

		// �ړI�n�_�ֈړ�
		if (owner_->MoveToTarget())
		{
			step = 0;

			return ActionBase::State::Complete;
		}

		// �v���C���[���G����������
		if (owner_->SearchTarget())
		{
			step = 0;
			// �p�j������Ԃ�
			return ActionBase::State::Complete;
		}
		break;
	}
	// ���s��
	return ActionBase::State::Run;
}

#pragma endregion



#pragma region �{�X���b�N

ActionBase<BossMech>::State MechIdleAction::Run(float deltaTime)
{
	switch (step)
	{
	case 0://������
		owner_->GetAnimator()->PlayAnimation("Idle");

		timer_ = 0.0f;

		step++;
		break;

	case 1:
		//�ҋ@��
		if (timer_ > Time)
		{
			step = 0;

			//�ҋ@����
			return ActionBase::State::Complete;
		}

		timer_ += deltaTime;
		break;
	}
	return ActionBase::State::Run;
}


ActionBase<BossMech>::State MechRunAttackAction::Run(float deltaTime)
{
	switch (step)
	{
	case 0://������
		owner_->GetAnimator()->PlayAnimation("Crouching");

		runTimer_ = 0.0f;

		step++;
		break;

	case 1:
		//����̍\�����ł����Ȃ烂�[�V�����ύX
		if (owner_->GetAnimator()->GetAnimationFinished())
		{
			//�A�j���[�V�����Đ�
			float transTime_ = 0.5f;
			owner_->GetAnimator()->PlayAnimation("Run", &transTime_);

			//�U���V�X�e��
			owner_->GetAttackerSystem()->Attack("Rush");

			step++;
		}
		break;
	case 2:

		owner_->RushAttackUpdate();
		
		if (runTimer_ > Max_Run_Time)
		{
			step = 0;

			owner_->SetMoveVec(Vector3::Zero);

			//�ҋ@����
			return ActionBase::State::Complete;
		}

		runTimer_ += deltaTime;
		break;
	}

	return ActionBase::State::Run;
}

#pragma endregion