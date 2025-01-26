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
class MechShotNormalBeamAction : public ActionBase<BossMech>
{
public:
	MechShotNormalBeamAction(BossMech* owner) : ActionBase(owner) {}
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

//拡散ビーム攻撃
class MechShotSuperHomingBeamAction : public ActionBase<BossMech>
{
public:
	MechShotSuperHomingBeamAction(BossMech* owner) : ActionBase(owner) {}
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
	float shotTime_ = 0.5f;
};

//ミサイル攻撃
class MechShotMissileAction : public ActionBase<BossMech>
{
public:
	MechShotMissileAction(BossMech* owner) : ActionBase(owner) {}
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
	float shotTime_ = 0.5f;
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

//敵からバックする
class MechBackToVitesseAction : public ActionBase<BossMech>
{
public:
	MechBackToVitesseAction(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;

private:
	enum class Step
	{
		Init,
		Move,
		Failed,
		Complete,
	};

	void Finalize();

private:
	float moveTime_ = 1.0f;
	float moveTimer_;
};

//敵に近接攻撃
class MechCombo01Action : public ActionBase<BossMech>
{
public:
	MechCombo01Action(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;
};

//敵に近接攻撃
class MechCombo02Action : public ActionBase<BossMech>
{
public:
	MechCombo02Action(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;
};

//敵に近接攻撃
class MechCombo03Action : public ActionBase<BossMech>
{
public:
	MechCombo03Action(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;
};

//死亡
class MechDieAction : public ActionBase<BossMech>
{
public:
	MechDieAction(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;
};

//横移動
class MechSideMoveAction : public ActionBase<BossMech>
{
public:
	MechSideMoveAction(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;
private:

	bool isMoveRight_ = true;

	float moveTime_ = 0.7f;
	float timer_;

};

//死亡
class MechTurnToVitesse : public ActionBase<BossMech>
{
public:
	MechTurnToVitesse(BossMech* owner) : ActionBase(owner) {}
	ActionBase::State Run(float deltaTime)override;
};
#pragma endregion
