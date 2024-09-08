#pragma once
#include "ActionBase.h"

class BaseEnemy;
class BotEnemy;

//BehaviorTreeは指定のクラスしか、登録できないので
//基底クラスとかの共通で使用できるようなクラスは作成できない

#pragma region ボットエネミー
// 通常攻撃
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
		AnimFinishWait,	// アニメーション終了待機
	};

	float timer_ = 0;
};

// 回避
class BotSideDodgeAction : public ActionBase<BotEnemy>
{
public:
	BotSideDodgeAction(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime)override;
private:
	//左右どちらに移動するか
	bool moveRight_;
	
	//回避時間
	float timer_ = 0.0f;
	float dodgeTime_;
};

// 待機
class BotIdleAction : public ActionBase<BotEnemy>
{
public:
	BotIdleAction(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime)override;

private:
	float timer_;
};

// 徘徊
class BotWonderActioin : public ActionBase<BotEnemy>
{
public:
	BotWonderActioin(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime)override;
};

#pragma endregion
