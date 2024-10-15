#include "JudgmentDerived.h"
#include "BotEnemy.h"

#include "Transform.h"
#include "MathHelper.h"

#include "BossMech.h"
#include "Vitesse.h"

using namespace AbyssEngine;

#pragma region ボットエネミー

// BattleNodeに遷移できるか判定
bool BotBattleJudgment::Judgment()
{
	//プレイヤーを見付けていて、一定距離離れていないならtrue
	if (owner_->GetIsTargetFind())
	{
		return owner_->SearchTargetWithimRange(10.0f);
	}

	//プレイヤーが見つかるか
	if (owner_->SearchTarget())
	{
		return true;
	}

	return false;
}

// AttackNodeに遷移できるか判定
bool AttackJudgment::Judgment()
{
	// 対象との距離を算出
	if (owner_->GetCanAttack())
	{
		// AttackNodeへ遷移できる
		return true;
	}
	return false;
}

bool DodgeJudgment::Judgment()
{
	//DodgeNodeに遷移できるか判定
	if (owner_->GetCanDodge())
	{
		return true;
	}
	return false;
}

// WanderNodeに遷移できるか判定
bool BotWanderJudgment::Judgment()
{
	// 目的地点までのXZ平面での距離判定
	Vector3 position = owner_->GetTransform()->GetPosition();
	Vector3 targetPosition = owner_->GetTargetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;

	// 目的地から離れている場合
	float radius = owner_->GetTerritoryRange();
	if (distSq > radius * radius)
	{
		return true;
	}

	return false;
}


#pragma endregion

#pragma region ボスメック

bool MechBattleJudgment::Judgment()
{
	if (const auto& target = owner_->GetTargetVitesse().lock())
	{
		//距離判定
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
