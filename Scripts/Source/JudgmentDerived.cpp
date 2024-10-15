#include "JudgmentDerived.h"
#include "BotEnemy.h"

#include "Transform.h"
#include "MathHelper.h"

#include "BossMech.h"
#include "Vitesse.h"

using namespace AbyssEngine;

#pragma region �{�b�g�G�l�~�[

// BattleNode�ɑJ�ڂł��邩����
bool BotBattleJudgment::Judgment()
{
	//�v���C���[�����t���Ă��āA��苗������Ă��Ȃ��Ȃ�true
	if (owner_->GetIsTargetFind())
	{
		return owner_->SearchTargetWithimRange(10.0f);
	}

	//�v���C���[�������邩
	if (owner_->SearchTarget())
	{
		return true;
	}

	return false;
}

// AttackNode�ɑJ�ڂł��邩����
bool AttackJudgment::Judgment()
{
	// �ΏۂƂ̋������Z�o
	if (owner_->GetCanAttack())
	{
		// AttackNode�֑J�ڂł���
		return true;
	}
	return false;
}

bool DodgeJudgment::Judgment()
{
	//DodgeNode�ɑJ�ڂł��邩����
	if (owner_->GetCanDodge())
	{
		return true;
	}
	return false;
}

// WanderNode�ɑJ�ڂł��邩����
bool BotWanderJudgment::Judgment()
{
	// �ړI�n�_�܂ł�XZ���ʂł̋�������
	Vector3 position = owner_->GetTransform()->GetPosition();
	Vector3 targetPosition = owner_->GetTargetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;

	// �ړI�n���痣��Ă���ꍇ
	float radius = owner_->GetTerritoryRange();
	if (distSq > radius * radius)
	{
		return true;
	}

	return false;
}


#pragma endregion

#pragma region �{�X���b�N

bool MechBattleJudgment::Judgment()
{
	if (const auto& target = owner_->GetTargetVitesse().lock())
	{
		//��������
		const Vector3 targetPos = target->GetTransform()->GetPosition();
		const Vector3 pos = owner_->GetTransform()->GetPosition();

		Vector3 toTarget = targetPos - pos;
		float lengthSq = toTarget.LengthSquared();
		if (lengthSq < attackRange_ * attackRange_)
		{
			return true;
		}
	}

	return false;
}

bool MechRunAttackJudgment::Judgment()
{


	return true;
}
#pragma endregion
