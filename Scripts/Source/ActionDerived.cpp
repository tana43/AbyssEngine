#include "ActionDerived.h"
#include "BaseEnemy.h"
#include "BotEnemy.h"
#include "Transform.h"
#include "Animator.h"
#include "Engine.h"
#include "BossMech.h"
#include "AttackerSystem.h"
#include "Vitesse.h"

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
		owner_->ToGroundMode();

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
		owner_->ToGroundMode();

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


ActionBase<BossMech>::State MechShotBeamAction::Run(float deltaTime)
{
	switch (step)
	{
	case static_cast<int>(Step::Init):

		//初期化
		owner_->GetAnimator()->PlayAnimation("Skill_01");
		owner_->ToFlightMode();

		timer_ = 0.0f;

		step++;
		break;

	case static_cast<int>(Step::Idle):
		//モーションがある程度進むまで待機
		if (timer_ > shotStartTime_)
		{
			step++;
		}

		break;
	case static_cast<int>(Step::Shot):

		//ビームを撃つ
		owner_->ShotHomingBeam();

		//if (owner_->GetAnimator()->GetAnimationFinished())
		if (timer_ > shotEndTime_)
		{
			step++;
		}
		break;
	case static_cast<int>(Step::End):

		//終了化
		step = 0;

		return ActionBase::State::Complete;

		break;
	}

	timer_ += deltaTime;

	return ActionBase::State::Run;
}

ActionBase<BossMech>::State MechFlyIdleAction::Run(float deltaTime)
{
	switch (step)
	{
	case 0://初期化
		owner_->GetAnimator()->PlayAnimation("Fly_Idle");
		owner_->ToFlightMode();

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

ActionBase<BossMech>::State MechMoveToVitesseAction::Run(float deltaTime)
{
	switch (step)
	{
	case static_cast<int>(Step::Init):
		//初期化
		owner_->GetAnimator()->PlayAnimation("Fly_Front_Start");

		step++;

		break;
	case static_cast<int>(Step::Start):

		if (const auto& v = owner_->GetTargetVitesse().lock())
		{
			if (owner_->MoveTo(v->GetTransform()->GetPosition()))
			{
				step = static_cast<int>(Step::Complete);
				break;
			}
		}

		if (owner_->GetAnimator()->GetAnimationFinished())
		{
			owner_->GetAnimator()->PlayAnimation("Fly_Front_Loop");

			step++;
		}

		break;
	case static_cast<int>(Step::Move):

		//ターゲットまで移動させる
		if (const auto& v = owner_->GetTargetVitesse().lock())
		{
			if (owner_->MoveTo(v->GetTransform()->GetPosition()))
			{
				step = static_cast<int>(Step::Complete);
			}
		}
		else
		{
			step = static_cast<int>(Step::Failed);
		}

		break;
	case static_cast<int>(Step::Complete):

		//急停止させる
		owner_->SetMoveVec(Vector3::Zero);
		owner_->SetVelocity(Vector3::Zero);

		step = 0;

		return ActionBase::State::Complete;

		break;
	case static_cast<int>(Step::Failed):

		step = 0;

		return ActionBase::State::Failed;

		break;
	}

	return ActionBase::State::Run;
}

ActionBase<BossMech>::State MechCombo01Action::Run(float deltaTime)
{
	switch (step)
	{
	case 0:
		//初期化
		owner_->GetAnimator()->PlayAnimation("Combo_01");

		//攻撃処理開始
		owner_->GetAttackerSystem()->Attack("Combo_01");

		step++;

		break;
	case 1:

		if (owner_->GetAnimator()->GetAnimationFinished())
		{
			step++;
		}

		break;
	case 2:

		step = 0;

		return ActionBase::State::Complete;

		break;
	}

	return ActionBase::State::Run;
}

#pragma endregion