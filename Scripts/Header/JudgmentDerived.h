#pragma once
#include "JudgmentBase.h"

class BaseEnemy;
class BotEnemy;
class BossMech;

#define Inheritance_Bot public JudgmentBase<BotEnemy>
#define Inheritance_Mech public JudgmentBase<BossMech>

//BehaviorTree�͎w��̃N���X�����A�o�^�ł��Ȃ��̂�
//���N���X�Ƃ��̋��ʂŎg�p�ł���悤�ȃN���X�͍쐬�ł��Ȃ�

#pragma region �{�b�g�G�l�~�[
// BattleNode�ɑJ�ڂł��邩����
class BotBattleJudgment : Inheritance_Bot
{
public:
	BotBattleJudgment(BotEnemy* owner) : JudgmentBase(owner) {}
	// ����
	bool Judgment();
};

// WanderNode�ɑJ�ڂł��邩����
class BotWanderJudgment : Inheritance_Bot
{
public:
	BotWanderJudgment(BotEnemy* owner) :JudgmentBase(owner) {}
	// ����
	bool Judgment();
};

// AttackNode�ɑJ�ڂł��邩����
class AttackJudgment : Inheritance_Bot
{
public:
	AttackJudgment(BotEnemy* owner) :JudgmentBase(owner) {}
	// ����
	bool Judgment();
};

// DodgeNode�ɑJ�ڂł��邩����
class DodgeJudgment : Inheritance_Bot
{
public:
	DodgeJudgment(BotEnemy* owner) :JudgmentBase(owner) {}
	// ����
	bool Judgment();
};
#pragma endregion

#pragma region �{�X���b�N
// BattleNode�ɑJ�ڂł��邩����
class MechBattleJudgment : Inheritance_Mech
{
public:
	MechBattleJudgment(BossMech* owner) : JudgmentBase(owner) {}
	// ����
	bool Judgment();
private:
	float attackRange_ = 500.0f;
};

// AttackNode�ɑJ�ڂł��邩����
class MechRunAttackJudgment : Inheritance_Mech
{
public:
	MechRunAttackJudgment(BossMech* owner) :JudgmentBase(owner) {}
	// ����
	bool Judgment();
};
#pragma endregion