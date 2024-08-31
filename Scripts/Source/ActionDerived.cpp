#include "ActionDerived.h"
#include "BaseEnemy.h"
#include "BotEnemy.h"

//using namespace AbyssEngine;

//// �U���s��
ActionBase<BotEnemy>::State BotAttackAction::Run()
{
    return ActionBase::State();
}

//// �p�j�s��
//ActionBase::State WanderAction::Run(float elapsedTime)
//{
//	switch (step)
//	{
//	case 0:
//		// �p�j���[�V�����ݒ�
//		owner->GetModel()->PlayAnimation(static_cast<int>(EnemyBlueSlime::EnemyAnimation::WalkFWD), true);
//		step++;
//		break;
//	case 1:
//		// �ړI�n�_�܂ł�XZ���ʂł̋�������
//		DirectX::XMFLOAT3 position = owner->GetPosition();
//		DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
//		float vx = targetPosition.x - position.x;
//		float vz = targetPosition.z - position.z;
//		float distSq = vx * vx + vz * vz;
//
//		// �ړI�n�֒�����
//		float radius = owner->GetRadius();
//		if (distSq < radius * radius)
//		{
//			step = 0;
//			// �p�j������Ԃ�
//			return ActionBase::State::Complete;
//		}
//
//		// �ړI�n�_�ֈړ�
//		owner->MoveToTarget(elapsedTime, 0.5f);
//
//		// �v���C���[���G����������
//		if (owner->SearchPlayer())
//		{
//			step = 0;
//			// �p�j������Ԃ�
//			return ActionBase::State::Complete;
//		}
//		break;
//	}
//	// ���s��
//	return ActionBase::State::Run;
//}
//
//// �ǐՍs��
//ActionBase::State PursuitAction::Run(float elapsedTime)
//{
//	float runTimer = owner->GetRunTimer();
//	switch (step)
//	{
//	case 0:
//		// �ڕW�n�_���v���C���[�ʒu�ɐݒ�
//		owner->SetTargetPosition(Player::Instance().GetPosition());
//		owner->SetRunTimer(Mathf::RandomRange(3.0f, 5.0f));
//		owner->GetModel()->PlayAnimation(static_cast<int>(EnemyBlueSlime::EnemyAnimation::RunFWD), true);
//		step++;
//		break;
//	case 1:
//		runTimer -= elapsedTime;
//		// �^�C�}�[�X�V
//		owner->SetRunTimer(runTimer);
//		// �ڕW�n�_���v���C���[�ʒu�ɐݒ�
//		owner->SetTargetPosition(Player::Instance().GetPosition());
//		// �ړI�n�_�ֈړ�
//		owner->MoveToTarget(elapsedTime, 1.0);
//
//		// �v���C���[�Ƃ̋������v�Z
//		DirectX::XMFLOAT3 position = owner->GetPosition();
//		DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
//
//		float vx = targetPosition.x - position.x;
//		float vy = targetPosition.y - position.y;
//		float vz = targetPosition.z - position.z;
//		float dist = sqrtf(vx * vx + vy * vy + vz * vz);
//		// �U���͈͂ɂ���Ƃ�
//		if (dist < owner->GetAttackRange())
//		{
//			step = 0;
//			// �ǐՐ�����Ԃ�
//			return ActionBase::State::Complete;
//		}
//		// �s�����Ԃ��߂�����
//		if (runTimer <= 0.0f)
//		{
//			step = 0;
//			// �ǐՎ��s��Ԃ�
//			return ActionBase::State::Failed;
//		}
//		break;
//	}
//	return ActionBase::State::Run;
//}
//
//// �ҋ@�s��
//ActionBase::State IdleAction::Run(float elapsedTime)
//{
//	float runTimer = owner->GetRunTimer();
//	switch (step)
//	{
//	case 0:
//		owner->SetRunTimer(Mathf::RandomRange(3.0f, 5.0f));
//		owner->GetModel()->PlayAnimation(static_cast<int>(EnemyBlueSlime::EnemyAnimation::IdleNormal), true);
//		step++;
//		break;
//	case 1:
//		runTimer -= elapsedTime;
//		// �^�C�}�[�X�V
//		owner->SetRunTimer(runTimer);
//
//		// �ҋ@���Ԃ��߂�����
//		if (runTimer <= 0.0f)
//		{
//			owner->SetRandomTargetPosition();
//			step = 0;
//			return ActionBase::State::Complete;
//		}
//
//		// �v���C���[����������
//		if (owner->SearchPlayer())
//		{
//			step = 0;
//			return ActionBase::State::Complete;
//		}
//		break;
//	}
//	return ActionBase::State::Run;
//}
//
//// �����s��
//ActionBase::State LeaveAction::Run(float elapsedTime)
//{
//	DirectX::XMFLOAT3 targetPosition;
//	switch (step)
//	{
//	case 0:
//		// �ڕW�n�_���v���C���[�Ɛ����΂̃x�N�g���~5�̈ʒu�Ɏw��
//		DirectX::XMVECTOR startPosition = DirectX::XMLoadFloat3(&Player::Instance().GetPosition());
//		DirectX::XMVECTOR endPosition = DirectX::XMLoadFloat3(&owner->GetPosition());
//
//		DirectX::XMVECTOR TargetPosition = DirectX::XMVectorSubtract(endPosition, startPosition);
//		TargetPosition = DirectX::XMVector3Normalize(TargetPosition);
//		TargetPosition = DirectX::XMVectorScale(TargetPosition, 5.0f);
//
//
//		DirectX::XMStoreFloat3(&targetPosition, TargetPosition);
//		targetPosition.x += owner->GetPosition().x;
//		targetPosition.y += owner->GetPosition().y;
//		targetPosition.z += owner->GetPosition().z;
//		owner->SetTargetPosition(targetPosition);
//
//		owner->GetModel()->PlayAnimation(static_cast<int>(EnemyBlueSlime::EnemyAnimation::RunFWD), true);
//		step++;
//		break;
//	case 1:
//
//		targetPosition = owner->GetTargetPosition();
//		// �ړI�n�_�ֈړ�
//		owner->MoveToTarget(elapsedTime, 1.0);
//
//		DirectX::XMFLOAT3 position = owner->GetPosition();
//		targetPosition = owner->GetTargetPosition();
//
//		float vx = targetPosition.x - position.x;
//		float vz = targetPosition.z - position.z;
//		float distSq = vx * vx + vz * vz;
//
//		// �ړI�n�֒�����
//		float radius = owner->GetRadius();
//		if (distSq < radius * radius)
//		{
//			step = 0;
//			return ActionBase::State::Complete;
//		}
//
//		break;
//	}
//
//	return ActionBase::State::Run;
//}
//// �񕜍s��
//ActionBase::State RecoverAction::Run(float elapsedTime)
//{
//	switch (step)
//	{
//	case 0:
//		owner->GetModel()->PlayAnimation(static_cast<int>(EnemyBlueSlime::EnemyAnimation::Taunt), false);
//		step++;
//		break;
//	case 1:
//		if (!owner->GetModel()->IsPlayAnimation())
//		{
//			owner->SetHealth(owner->GetMaxHealth());
//			step = 0;
//			return ActionBase::State::Complete;
//		}
//		break;
//	}
//
//	return ActionBase::State::Run;
//
//}

