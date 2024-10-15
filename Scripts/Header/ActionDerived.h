#pragma once
#include "ActionBase.h"

class BaseEnemy;
class BotEnemy;
class BossMech;

//BehaviorTree�͎w��̃N���X�����A�o�^�ł��Ȃ��̂�
//���N���X�Ƃ��̋��ʂŎg�p�ł���悤�ȃN���X�͍쐬�ł��Ȃ�

#pragma region �{�b�g�G�l�~�[
// �ʏ�U��
class BotAttackAction : public ActionBase<BotEnemy>
{
public:
	BotAttackAction(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime)override;
private:

	enum class Step
	{
		Init,
		LockOn,
		LockIdle,
		Shot,
		AnimFinishWait,	// �A�j���[�V�����I���ҋ@
	};

	float timer_ = 0;
};

// ���
class BotSideDodgeAction : public ActionBase<BotEnemy>
{
public:
	BotSideDodgeAction(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime)override;
private:
	//���E�ǂ���Ɉړ����邩
	bool moveRight_;
	
	//�������
	float timer_ = 0.0f;
	float dodgeTime_;
};

// �ҋ@
class BotIdleAction : public ActionBase<BotEnemy>
{
public:
	BotIdleAction(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime)override;

private:
	float timer_;
};

// �p�j
class BotWonderActioin : public ActionBase<BotEnemy>
{
public:
	BotWonderActioin(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime)override;
};

#pragma endregion

#pragma region �{�X���b�N
// �ҋ@
class MechIdleAction : public ActionBase<BossMech>
{
public:
	MechIdleAction(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime)override;

private:
	float timer_ = 0.0f;
	const float Time = 1.0f;
};

// �U��
class MechRunAttackAction : public ActionBase<BossMech>
{
public:
	MechRunAttackAction(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime)override;

private:
	float runTimer_;

	//�ő�̃_�b�V������
	const float Max_Run_Time = 4.0f;
};

//�]�|�Ƃ��͗~������

#pragma endregion
