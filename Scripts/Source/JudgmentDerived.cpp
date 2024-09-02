#include "JudgmentDerived.h"
#include "BotEnemy.h"

#include "Transform.h"
#include "MathHelper.h"

using namespace AbyssEngine;

#pragma region �G�l�~�[����

// BattleNode�ɑJ�ڂł��邩����
bool BattleJudgment::Judgment()
{
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
#pragma endregion


#pragma region �{�b�g�G�l�~�[
// WanderNode�ɑJ�ڂł��邩����
bool WanderJudgment::Judgment()
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
