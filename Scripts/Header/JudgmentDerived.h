#pragma once
#include "JudgmentBase.h"

#pragma region �G�l�~�[�{�b�g
// BattleNode�ɑJ�ڂł��邩����
template <class T>
class BattleJudgment : public JudgmentBase<T>
{
public:
	BattleJudgment(T* owner) :JudgmentBase(owner) {}
	// ����
	bool Judgment();
};

// AttackNode�ɑJ�ڂł��邩����
template <class T>
class AttackJudgment : public JudgmentBase<T>
{
public:
	AttackJudgment(T* owner) :JudgmentBase(owner) {}
	// ����
	bool Judgment();
};

// SkillNode�ɑJ�ڂł��邩����
template <class T>
class SkillShotJudgment : public JudgmentBase<T>
{
public:
	SkillShotJudgment(T* owner) :JudgmentBase(owner) {}
	// ����
	bool Judgment();
};

// WanderNode�ɑJ�ڂł��邩����
template <class T>
class WanderJudgment : public JudgmentBase<T>
{
public:
	WanderJudgment(T* owner) :JudgmentBase(owner) {}
	// ����
	bool Judgment();
};


// Escape�ɑJ�ڂł��邩����
template <class T>
class EscapeJudgment : public JudgmentBase<T>
{
public:
	EscapeJudgment(T* owner) :JudgmentBase(owner) {}
	// ����
	bool Judgment();
};
#pragma endregion
