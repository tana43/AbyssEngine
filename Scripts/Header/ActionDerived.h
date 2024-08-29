#pragma once
#include "ActionBase.h"
#include "EnemyBlueSlime.h"

// �ʏ�U��
class AttackAction : public ActionBase
{
public:
	AttackAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// �X�L���U���s��
class SkillAction : public ActionBase
{
public:
	SkillAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// �ǐՍs��
class PursuitAction : public ActionBase
{
public:
	PursuitAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// �p�j�s��
class WanderAction : public ActionBase
{
public:
	WanderAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// �ҋ@�s��
class IdleAction : public ActionBase
{
public:
	IdleAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// �����s��
class LeaveAction : public ActionBase
{
public:
	LeaveAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// �񕜍s��
class RecoverAction : public ActionBase
{
public:
	RecoverAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};