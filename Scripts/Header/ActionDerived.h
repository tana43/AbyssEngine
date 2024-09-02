#pragma once
#include "ActionBase.h"

class BaseEnemy;
class BotEnemy;

#pragma region エネミー共通
// 通常攻撃
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

// 回避
class BotDodgeAction : public ActionBase<BotEnemy>
{
public:
	BotDodgeAction(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run()override;
};
#pragma endregion
