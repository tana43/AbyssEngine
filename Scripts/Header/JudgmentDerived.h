#pragma once
#include "JudgmentBase.h"

#pragma region ƒGƒlƒ~[ƒ{ƒbƒg
// BattleNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
template <class T>
class BattleJudgment : public JudgmentBase<T>
{
public:
	BattleJudgment(T* owner) :JudgmentBase(owner) {}
	// ”»’è
	bool Judgment();
};

// AttackNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
template <class T>
class AttackJudgment : public JudgmentBase<T>
{
public:
	AttackJudgment(T* owner) :JudgmentBase(owner) {}
	// ”»’è
	bool Judgment();
};

// SkillNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
template <class T>
class SkillShotJudgment : public JudgmentBase<T>
{
public:
	SkillShotJudgment(T* owner) :JudgmentBase(owner) {}
	// ”»’è
	bool Judgment();
};

// WanderNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
template <class T>
class WanderJudgment : public JudgmentBase<T>
{
public:
	WanderJudgment(T* owner) :JudgmentBase(owner) {}
	// ”»’è
	bool Judgment();
};


// Escape‚É‘JˆÚ‚Å‚«‚é‚©”»’è
template <class T>
class EscapeJudgment : public JudgmentBase<T>
{
public:
	EscapeJudgment(T* owner) :JudgmentBase(owner) {}
	// ”»’è
	bool Judgment();
};
#pragma endregion
