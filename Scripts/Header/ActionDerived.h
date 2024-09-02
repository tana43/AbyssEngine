#pragma once
#include "ActionBase.h"

class BaseEnemy;
class BotEnemy;

#pragma region �G�l�~�[����
// �ʏ�U��
class BotAttackAction : public ActionBase<BotEnemy>
{
public:
	BotAttackAction(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run()override;
private:
	enum class Step
	{
		Init,
		LockOn,
		LockIdle,
		Shot
	};

	float timer_ = 0;
};

// ���
class BotSideDodgeAction : public ActionBase<BotEnemy>
{
public:
	BotSideDodgeAction(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run()override;
private:
	//���E�ǂ���Ɉړ����邩
	bool moveRight_;
};

// �ҋ@
class BotIdleAction : public ActionBase<BotEnemy>
{
public:
	BotIdleAction(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run()override;

private:
	float timer_;
};

#pragma endregion
