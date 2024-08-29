#pragma once
#include "ActionBase.h"

// �ʏ�U��
template <class T>
class AttackAction : public ActionBase<T>
{
public:
	AttackAction(T* owner) :ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime);
};

// �X�L���U���s��
template <class T>
class SkillAction : public ActionBase<T>
{
public:
	SkillAction(T* owner) :ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime);
};

// �ǐՍs��
template <class T>
class PursuitAction : public ActionBase<T>
{
public:
	PursuitAction(T* owner) :ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime);
};

// �p�j�s��
template <class T>
class WanderAction : public ActionBase<T>
{
public:
	WanderAction(T* owner) :ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime);
};

// �ҋ@�s��
template <class T>
class IdleAction : public ActionBase<T>
{
public:
	IdleAction(T* owner) :ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime);
};

// �����s��
template <class T>
class LeaveAction : public ActionBase<T>
{
public:
	LeaveAction(T* owner) :ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime);
};

// �񕜍s��
template <class T>
class RecoverAction : public ActionBase<T>
{
public:
	RecoverAction(T* owner) :ActionBase(owner) {}
	ActionBase::State Run(float elapsedTime);
};