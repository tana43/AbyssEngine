#include "ActionDerived.h"
#include "BaseEnemy.h"
#include "BotEnemy.h"
#include "Transform.h"
#include "Animator.h"
#include "Engine.h"

using namespace AbyssEngine;

// 攻撃行動
ActionBase<BotEnemy>::State BotAttackAction::Run()
{
	auto state = ActionBase::State::Run;

	switch (step)
	{
	case static_cast<int>(Step::Init)://初期化
		//タイマーリセット
		timer_ = 0;

		//アニメーション再生
		owner_->GetAnimator()->PlayAnimation(static_cast<int>(BotEnemy::AnimState::Attack));

		//次のステップへ
		step++;

		break;
	case static_cast<int>(Step::LockOn):

		//ターゲットをロックオン
		owner_->LockOn();

		//ターゲットを正確にロックオンする時間が経過したら次のステップへ
		if (timer_ > owner_->GetLockOnTime())
		{
			//タイマーリセット
			timer_ = 0;

			step++;
		}

		//タイマー更新
		timer_ += Time::deltaTime_;

		break;
	case static_cast<int>(Step::LockIdle):

		//待機時間が経過したら次のステップへ
		if (timer_ > owner_->GetLockOnShotTime())
		{
			//タイマーリセット
			timer_ = 0;

			step++;
		}

		//タイマー更新
		timer_ += Time::deltaTime_;
		break;

	case static_cast<int>(Step::Shot):

		//射撃
		owner_->Shot();

		//行動完了
		state = ActionBase::State::Complete;

		//撃ち終わった後に余韻あった方がいいかも.........？

		break;
	}
	

    return state;
}

//// 徘徊行動
//ActionBase::State WanderAction::Run(float elapsedTime)
//{
//	switch (step)
//	{
//	case 0:
//		// 徘徊モーション設定
//		owner->GetModel()->PlayAnimation(static_cast<int>(EnemyBlueSlime::EnemyAnimation::WalkFWD), true);
//		step++;
//		break;
//	case 1:
//		// 目的地点までのXZ平面での距離判定
//		DirectX::XMFLOAT3 position = owner->GetPosition();
//		DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
//		float vx = targetPosition.x - position.x;
//		float vz = targetPosition.z - position.z;
//		float distSq = vx * vx + vz * vz;
//
//		// 目的地へ着いた
//		float radius = owner->GetRadius();
//		if (distSq < radius * radius)
//		{
//			step = 0;
//			// 徘徊成功を返す
//			return ActionBase::State::Complete;
//		}
//
//		// 目的地点へ移動
//		owner->MoveToTarget(elapsedTime, 0.5f);
//
//		// プレイヤー索敵成功したら
//		if (owner->SearchPlayer())
//		{
//			step = 0;
//			// 徘徊成功を返す
//			return ActionBase::State::Complete;
//		}
//		break;
//	}
//	// 実行中
//	return ActionBase::State::Run;
//}
//
//// 追跡行動
//ActionBase::State PursuitAction::Run(float elapsedTime)
//{
//	float runTimer = owner->GetRunTimer();
//	switch (step)
//	{
//	case 0:
//		// 目標地点をプレイヤー位置に設定
//		owner->SetTargetPosition(Player::Instance().GetPosition());
//		owner->SetRunTimer(Mathf::RandomRange(3.0f, 5.0f));
//		owner->GetModel()->PlayAnimation(static_cast<int>(EnemyBlueSlime::EnemyAnimation::RunFWD), true);
//		step++;
//		break;
//	case 1:
//		runTimer -= elapsedTime;
//		// タイマー更新
//		owner->SetRunTimer(runTimer);
//		// 目標地点をプレイヤー位置に設定
//		owner->SetTargetPosition(Player::Instance().GetPosition());
//		// 目的地点へ移動
//		owner->MoveToTarget(elapsedTime, 1.0);
//
//		// プレイヤーとの距離を計算
//		DirectX::XMFLOAT3 position = owner->GetPosition();
//		DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
//
//		float vx = targetPosition.x - position.x;
//		float vy = targetPosition.y - position.y;
//		float vz = targetPosition.z - position.z;
//		float dist = sqrtf(vx * vx + vy * vy + vz * vz);
//		// 攻撃範囲にいるとき
//		if (dist < owner->GetAttackRange())
//		{
//			step = 0;
//			// 追跡成功を返す
//			return ActionBase::State::Complete;
//		}
//		// 行動時間が過ぎた時
//		if (runTimer <= 0.0f)
//		{
//			step = 0;
//			// 追跡失敗を返す
//			return ActionBase::State::Failed;
//		}
//		break;
//	}
//	return ActionBase::State::Run;
//}
//
//// 待機行動
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
//		// タイマー更新
//		owner->SetRunTimer(runTimer);
//
//		// 待機時間が過ぎた時
//		if (runTimer <= 0.0f)
//		{
//			owner->SetRandomTargetPosition();
//			step = 0;
//			return ActionBase::State::Complete;
//		}
//
//		// プレイヤーを見つけた時
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
//// 逃走行動
//ActionBase::State LeaveAction::Run(float elapsedTime)
//{
//	DirectX::XMFLOAT3 targetPosition;
//	switch (step)
//	{
//	case 0:
//		// 目標地点をプレイヤーと正反対のベクトル×5の位置に指定
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
//		// 目的地点へ移動
//		owner->MoveToTarget(elapsedTime, 1.0);
//
//		DirectX::XMFLOAT3 position = owner->GetPosition();
//		targetPosition = owner->GetTargetPosition();
//
//		float vx = targetPosition.x - position.x;
//		float vz = targetPosition.z - position.z;
//		float distSq = vx * vx + vz * vz;
//
//		// 目的地へ着いた
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
//// 回復行動
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

ActionBase<BotEnemy>::State BotSideDodgeAction::Run()
{
	switch (step)
	{
	case 0:

		//コロコロ回転するアニメーション再生
		owner_->GetAnimator()->PlayAnimation(static_cast<int>(BotEnemy::AnimState::Rolling));

		//乱数で左右どちらに進むかを設定
		moveRight_ = rand() % 2;

		//次のステップへ
		step++;

		break;

	case 1:

		//ターゲットを中心に回転するように移動
		owner_->SideMove(owner_->GetTargetActor()->GetTransform()->GetPosition(),moveRight_);

		if (owner_->GetAnimator()->GetAnimationFinished())
		{
			//行動完了
			return ActionBase::State::Complete;
		}
		break;
	}


	return ActionBase::State::Run;
}

ActionBase<BotEnemy>::State BotIdleAction::Run()
{
	switch (step)
	{
	case 0://初期化

		//待機アニメーション再生
		owner_->GetAnimator()->PlayAnimation(static_cast<int>(BotEnemy::AnimState::Idle));

		//タイマーリセット
		timer_ = 0.0f;

		//次のステップへ
		step++;

		break;

	case 1:
		//TODO : 敵待機行動　仮で４秒間に設定してる
		if (timer_ > 4.0f)
		{
			//行動完了
			return State::Complete;
		}

		//タイマー更新
		timer_ += Time::deltaTime_;
		break;
	}

	return State::Run;
}
