#pragma once
#include "JudgmentBase.h"

#pragma region エネミーボット
// BattleNodeに遷移できるか判定
template <class T>
class BattleJudgment : public JudgmentBase<T>
{
public:
	BattleJudgment(T* owner) :JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

// AttackNodeに遷移できるか判定
template <class T>
class AttackJudgment : public JudgmentBase<T>
{
public:
	AttackJudgment(T* owner) :JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

// SkillNodeに遷移できるか判定
template <class T>
class SkillShotJudgment : public JudgmentBase<T>
{
public:
	SkillShotJudgment(T* owner) :JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};

// WanderNodeに遷移できるか判定
template <class T>
class WanderJudgment : public JudgmentBase<T>
{
public:
	WanderJudgment(T* owner) :JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};


// Escapeに遷移できるか判定
template <class T>
class EscapeJudgment : public JudgmentBase<T>
{
public:
	EscapeJudgment(T* owner) :JudgmentBase(owner) {}
	// 判定
	bool Judgment();
};
#pragma endregion
