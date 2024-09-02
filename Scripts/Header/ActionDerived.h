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
class BotSideDodgeAction : public ActionBase<BotEnemy>
{
public:
	BotSideDodgeAction(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run()override;
private:
	//左右どちらに移動するか
	bool moveRight_;
};

// 待機
class BotIdleAction : public ActionBase<BotEnemy>
{
public:
	BotIdleAction(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run()override;

private:
	float timer_;
};

#pragma endregion
