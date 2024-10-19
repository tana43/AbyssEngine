#include "ActionDerived.h"
#include "BaseEnemy.h"
#include "BotEnemy.h"
#include "Transform.h"
#include "Animator.h"
#include "Engine.h"
#include "BossMech.h"
#include "AttackerSystem.h"

using namespace AbyssEngine;

#pragma region ボットエネミー

// 攻撃行動
ActionBase<BotEnemy>::State BotAttackAction::Run(float deltaTime)
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
		timer_ += deltaTime;

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
		timer_ += deltaTime;
		break;

	case static_cast<int>(Step::Shot):


		//射撃
		owner_->Shot();

		//撃ち終わった後に余韻あった方がいいかも.........？

		step++;

		break;

	case static_cast<int>(Step::AnimFinishWait):

		if (owner_->GetAnimator()->GetAnimationFinished())
		{
			//行動完了
			state = ActionBase::State::Complete;

			//ステップリセット
			step = 0;
		}
		break;
	}


	return state;
}


ActionBase<BotEnemy>::State BotSideDodgeAction::Run(float deltaTime)
{
	switch (step)
	{
	case 0:

		//コロコロ回転するアニメーション再生
		owner_->GetAnimator()->PlayAnimation(static_cast<int>(BotEnemy::AnimState::Rolling));

		//乱数で左右どちらに進むかを設定
		moveRight_ = rand() % 2;

		//回避時間設定
		dodgeTime_ = 1.5 + (static_cast<float>(rand() % 10) / 10.0f);

		//次のステップへ
		step++;

		break;

	case 1:

		//ターゲットを中心に回転するように移動
		owner_->SideMove(owner_->GetTargetActor()->GetTransform()->GetPosition(), moveRight_);

		//リロードが完了しているなら終了
		//if (owner_->GetAnimator()->GetAnimationFinished())
		if (timer_ > dodgeTime_)
		{
			step = 0;

			timer_ = 0;

			//行動完了
			return ActionBase::State::Complete;
		}

		timer_ += deltaTime;

		break;
	}


	return ActionBase::State::Run;
}

ActionBase<BotEnemy>::State BotIdleAction::Run(float deltaTime)
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
			step = 0;

			//行動完了
			return State::Complete;
		}

		//タイマー更新
		timer_ += deltaTime;
		break;
	}

	return State::Run;
}

//徘徊行動
ActionBase<BotEnemy>::State BotWonderActioin::Run(float deltaTime)
{
	switch (step)
	{
	case 0:
		// 徘徊モーション設定
		owner_->GetAnimator()->PlayAnimation(static_cast<int>(BotEnemy::AnimState::Walk));

		// 目標座標設定
		owner_->SetRandomTargetPosition();

		step++;
		break;
	case 1:

		// 目的地点へ移動
		if (owner_->MoveToTarget())
		{
			step = 0;

			return ActionBase::State::Complete;
		}

		// プレイヤー索敵成功したら
		if (owner_->SearchTarget())
		{
			step = 0;
			// 徘徊成功を返す
			return ActionBase::State::Complete;
		}
		break;
	}
	// 実行中
	return ActionBase::State::Run;
}

#pragma endregion



#pragma region ボスメック

ActionBase<BossMech>::State MechIdleAction::Run(float deltaTime)
{
	switch (step)
	{
	case 0://初期化
		owner_->GetAnimator()->PlayAnimation("Idle");

		timer_ = 0.0f;

		step++;
		break;

	case 1:
		//待機中
		if (timer_ > Time)
		{
			step = 0;

			//待機完了
			return ActionBase::State::Complete;
		}

		timer_ += deltaTime;
		break;
	}
	return ActionBase::State::Run;
}


ActionBase<BossMech>::State MechRunAttackAction::Run(float deltaTime)
{
	switch (step)
	{
	case 0://初期化
		owner_->GetAnimator()->PlayAnimation("Crouching");

		runTimer_ = 0.0f;

		step++;
		break;

	case 1:
		//走りの構えができたならモーション変更
		if (owner_->GetAnimator()->GetAnimationFinished())
		{
			//アニメーション再生
			float transTime_ = 0.5f;
			owner_->GetAnimator()->PlayAnimation("Run", &transTime_);

			//攻撃システム
			owner_->GetAttackerSystem()->Attack("Rush");

			step++;
		}
		break;
	case 2:

		owner_->RushAttackUpdate();
		
		if (runTimer_ > Max_Run_Time)
		{
			step = 0;

			owner_->SetMoveVec(Vector3::Zero);

			//待機完了
			return ActionBase::State::Complete;
		}

		runTimer_ += deltaTime;
		break;
	}

	return ActionBase::State::Run;
}

#pragma endregion