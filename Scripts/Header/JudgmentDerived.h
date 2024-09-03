#pragma once
#include "JudgmentBase.h"

class BaseEnemy;
class BotEnemy;

#define Inheritance_Bot public JudgmentBase<BotEnemy>

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