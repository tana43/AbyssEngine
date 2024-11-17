#pragma once
#include "JudgmentBase.h"

class BaseEnemy;
class BotEnemy;
class BossMech;

#define Inheritance_Bot public JudgmentBase<BotEnemy>
#define Inheritance_Mech public JudgmentBase<BossMech>

//BehaviorTreeは指定のクラスしか、登録できないので
//基底クラスとかの共通で使用できるようなクラスは作成できない

#pragma region ボットエネミー
// BattleNodeに遷移できるか判定
class BotBattleJudgment : Inheritance_Bot
{
public:
	BotBattleJudgment(BotEnemy* owner) : JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

// WanderNodeに遷移できるか判定
class BotWanderJudgment : Inheritance_Bot
{
public:
	BotWanderJudgment(BotEnemy* owner) :JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

// AttackNodeに遷移できるか判定
class AttackJudgment : Inheritance_Bot
{
public:
	AttackJudgment(BotEnemy* owner) :JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

// DodgeNodeに遷移できるか判定
class DodgeJudgment : Inheritance_Bot
{
public:
	DodgeJudgment(BotEnemy* owner) :JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};
#pragma endregion

#pragma region ボスメック
// BattleNodeに遷移できるか判定
class MechBattleJudgment : Inheritance_Mech
{
public:
	MechBattleJudgment(BossMech* owner) : JudgmentBase(owner) {}
	// 判定
	bool Judgment();
private:
	float attackRange_ = 500.0f;
};

// AttackNodeに遷移できるか判定
class MechRunAttackJudgment : Inheritance_Mech
{
public:
	MechRunAttackJudgment(BossMech* owner) :JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

class MechShotBeamJudgment : Inheritance_Mech
{
public:
	MechShotBeamJudgment(BossMech* owner) :JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

//飛行モードか
class MechFlyJudgment : Inheritance_Mech
{
public:
	MechFlyJudgment(BossMech* owner) : JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

//地上モードか
class MechGroundJudgment : Inheritance_Mech
{
public:
	MechGroundJudgment(BossMech* owner) : JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

//死亡しているか
class MechDieJudgment : Inheritance_Mech
{
public:
	MechDieJudgment(BossMech* owner) : JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

//ターゲットは近距離か
class MechNearRangeJudgment : Inheritance_Mech
{
public:
	MechNearRangeJudgment(BossMech* owner) : JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

//ターゲットは中距離か
class MechMiddleRangeJudgment : Inheritance_Mech
{
public:
	MechMiddleRangeJudgment(BossMech* owner) : JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

//ターゲットと近すぎないか
class MechNearestRangeJudgment : Inheritance_Mech
{
public:
	MechNearestRangeJudgment(BossMech* owner) : JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};
#pragma endregion