#pragma once
#include "ActionBase.h"

// 通常攻撃
template <class T>
class AttackAction : public ActionBase<T>
{
public:
	AttackAction(T* owner) :ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime);
};

// スキル攻撃行動
template <class T>
class SkillAction : public ActionBase<T>
{
public:
	SkillAction(T* owner) :ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime);
};

// 追跡行動
template <class T>
class PursuitAction : public ActionBase<T>
{
public:
	PursuitAction(T* owner) :ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime);
};

// 徘徊行動
template <class T>
class WanderAction : public ActionBase<T>
{
public:
	WanderAction(T* owner) :ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime);
};

// 待機行動
template <class T>
class IdleAction : public ActionBase<T>
{
public:
	IdleAction(T* owner) :ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime);
};

// 逃走行動
template <class T>
class LeaveAction : public ActionBase<T>
{
public:
	LeaveAction(T* owner) :ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime);
};

// 回復行動
template <class T>
class RecoverAction : public ActionBase<T>
{
public:
	RecoverAction(T* owner) :ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime);
};