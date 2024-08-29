#include "JudgmentDerived.h"
#include "Player.h"
#include "Mathf.h"


// BattleNode�ɑJ�ڂł��邩����
bool BattleJudgment::Judgment()
{
	// �v���C���[�������邩
	if (owner->SearchPlayer())
	{
		return true;
	}
	return false;
}

// AttackNode�ɑJ�ڂł��邩����
bool AttackJudgment::Judgment()
{
	// �ΏۂƂ̋������Z�o
	DirectX::XMFLOAT3 position = owner->GetPosition();
	DirectX::XMFLOAT3 targetPosition = Player::Instance().GetPosition();

	float vx = targetPosition.x - position.x;
	float vy = targetPosition.y - position.y;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist < owner->GetAttackRange())
	{
		// AttackNode�֑J�ڂł���
		return true;
	}
	return false;
}



// SkillNode�ɑJ�ڂł��邩����
bool SkillShotJudgment::Judgment()
{
	// hp�������ȉ��̎�skill�����\
	int health = owner->GetHealth();
	int baseHealth = static_cast<int>(owner->GetMaxHealth() * 0.8);
	if (health < baseHealth)
	{
		// SkillNode�֑J�ڂł���
		return true;
	}
	return false;
}

// WanderNode�ɑJ�ڂł��邩����
bool WanderJudgment::Judgment()
{
	// �ړI�n�_�܂ł�XZ���ʂł̋�������
	DirectX::XMFLOAT3 position = owner->GetPosition();
	DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;

	// �ړI�n���痣��Ă���ꍇ
	float radius = owner->GetRadius();
	if (distSq > radius * radius)
	{
		return true;
	}

	return false;
}

bool EscapeJudgment::Judgment()
{
	if (owner->GetHealth() < (owner->GetMaxHealth() * 0.5))
	{
		return true;
	}
	return false;
}