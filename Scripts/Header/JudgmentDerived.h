#pragma once
#include "JudgmentBase.h"

class BaseEnemy;
class BotEnemy;

#pragma region エネミー共通
// AttackNodeに遷移できるか判定
class AttackJudgment : public JudgmentBase<BotEnemy>
{
public:
	AttackJudgment(BotEnemy* owner) :JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

// DodgeNodeに遷移できるか判定
class DodgeJudgment : public JudgmentBase<BaseEnemy>
{
public:
	DodgeJudgment(BaseEnemy* owner) :JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};
#pragma endregion

#pragma region ボットエネミー
// BattleNodeに遷移できるか判定
class BattleJudgment : public JudgmentBase<BotEnemy>
{
public:
	BattleJudgment(BotEnemy* owner) : JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

// WanderNodeに遷移できるか判定
class WanderJudgment : public JudgmentBase<BotEnemy>
{
public:
	WanderJudgment(BotEnemy* owner) :JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};
#pragma endregion