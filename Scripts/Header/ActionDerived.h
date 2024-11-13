#pragma once
#include "ActionBase.h"

class BaseEnemy;
class BotEnemy;
class BossMech;

//BehaviorTreeは指定のクラスしか、登録できないので
//基底クラスとかの共通で使用できるようなクラスは作成できない

#pragma region ボットエネミー
// 通常攻撃
class BotAttackAction : public ActionBase<BotEnemy>
{
public:
	BotAttackAction(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;
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
	ActionBase::State Run(float deltaTime)override;
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
	ActionBase::State Run(float deltaTime)override;

private:
	float timer_;
};

// 徘徊
class BotWonderActioin : public ActionBase<BotEnemy>
{
public:
	BotWonderActioin(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;
};

#pragma endregion

#pragma region ボスメック
// 待機
class MechIdleAction : public ActionBase<BossMech>
{
public:
	MechIdleAction(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;

private:
	float timer_ = 0.0f;
	const float Time = 1.0f;
};

// 待機
class MechFlyIdleAction : public ActionBase<BossMech>
{
public:
	MechFlyIdleAction(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;

private:
	float timer_ = 0.0f;
	const float Time = 1.0f;
};

// 攻撃
class MechRunAttackAction : public ActionBase<BossMech>
{
public:
	MechRunAttackAction(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;

private:
	float runTimer_;

	//最大のダッシュ時間
	const float Max_Run_Time = 4.0f;
};

//ビーム攻撃
class MechShotBeamAction : public ActionBase<BossMech>
{
public:
	MechShotBeamAction(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;

private:
	enum class Step
	{
		Init,
		Idle,
		Shot,
		End,
	};

	float timer_ = 0.0f;
	float shotStartTime_ = 0.5f;
	float shotEndTime_ = 1.0f;
};

//プレイヤーの目の前まで移動する
class MechMoveToVitesseAction : public ActionBase<BossMech>
{
public:
	MechMoveToVitesseAction(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;

private:
	enum class Step
	{
		Init,
		Start,
		Move,
		Failed,
		Complete,
	};
};

//敵に近接攻撃
class MechCombo01Action : public ActionBase<BossMech>
{
public:
	MechCombo01Action(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;
};
#pragma endregion
