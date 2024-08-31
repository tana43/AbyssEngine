#pragma once
#include "JudgmentBase.h"

class BaseEnemy;
class BotEnemy;

#pragma region �G�l�~�[����
// AttackNode�ɑJ�ڂł��邩����
class AttackJudgment : public JudgmentBase<BotEnemy>
{
public:
	AttackJudgment(BotEnemy* owner) :JudgmentBase(owner) {}
	// ����
	bool Judgment();
};

// DodgeNode�ɑJ�ڂł��邩����
class DodgeJudgment : public JudgmentBase<BaseEnemy>
{
public:
	DodgeJudgment(BaseEnemy* owner) :JudgmentBase(owner) {}
	// ����
	bool Judgment();
};
#pragma endregion

#pragma region �{�b�g�G�l�~�[
// BattleNode�ɑJ�ڂł��邩����
class BattleJudgment : public JudgmentBase<BotEnemy>
{
public:
	BattleJudgment(BotEnemy* owner) : JudgmentBase(owner) {}
	// ����
	bool Judgment();
};

// WanderNode�ɑJ�ڂł��邩����
class WanderJudgment : public JudgmentBase<BotEnemy>
{
public:
	WanderJudgment(BotEnemy* owner) :JudgmentBase(owner) {}
	// ����
	bool Judgment();
};
#pragma endregion