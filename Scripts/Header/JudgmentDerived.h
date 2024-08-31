//#pragma once
//#include "JudgmentBase.h"
//#include "EnemyBot.h"
//
//#pragma region エネミーボット
//// BattleNodeに遷移できるか判定
//class BattleJudgment : public JudgmentBase<EnemyBot>
//{
//public:
//	BattleJudgment(EnemyBot* owner) : JudgmentBase(owner) {}
//	// 判定
//	bool Judgment();
//};
//
//// AttackNodeに遷移できるか判定
//class AttackJudgment : public JudgmentBase<EnemyBot>
//{
//public:
//	AttackJudgment(EnemyBot* owner) :JudgmentBase(owner) {}
//	// 判定
//	bool Judgment();
//};
//
//// SkillNodeに遷移できるか判定
//template <class T>
//class SkillShotJudgment : public JudgmentBase<T>
//{
//public:
//	SkillShotJudgment(T* owner) :JudgmentBase(owner) {}
//	// 判定
//	bool Judgment();
//};
//
//// WanderNodeに遷移できるか判定
//template <class T>
//class WanderJudgment : public JudgmentBase<T>
//{
//public:
//	WanderJudgment(T* owner) :JudgmentBase(owner) {}
//	// 判定
//	bool Judgment();
//};
//
//
//// Escapeに遷移できるか判定
//template <class T>
//class EscapeJudgment : public JudgmentBase<T>
//{
//public:
//	EscapeJudgment(T* owner) :JudgmentBase(owner) {}
//	// 判定
//	bool Judgment();
//};
//#pragma endregion
