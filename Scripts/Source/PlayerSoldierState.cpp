#include "PlayerSoldierState.h"
#include "PlayerSoldier.h"
#include "Vitesse.h"
#include "Transform.h"
#include "Input.h"

#include "Engine.h"

using namespace AbyssEngine;

void SoldierState::Move::Initialize()
{
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Move));
}

void SoldierState::Move::Update()
{
    owner_->InputMove();

    //武器の位置を変更
    owner_->ChangeSocketTransformLinear(0.1f,
        owner_->Weapon_Offset_Move.pos,
        owner_->Weapon_Offset_Move.rot
    );

    //ヴィテスへ搭乗
    if (Input::GameSupport::GetBoardingButton())
    {
        owner_->BoardingTheVitesse();
    }

    //エイムボタンが押されているなら遷移
    if (Input::GameSupport::GetAimButton())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Aim));
    }
}

void SoldierState::Move::Finalize()
{

}

void SoldierState::Aim::Initialize()
{
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Aim));

    //自動振り向きをオフに
    owner_->SetEnableAutoTurn(false);

    //カメラを寄らせる
    owner_->GetCamera()->Zoom(zoomParam);
}

void SoldierState::Aim::Update()
{
    //武器の位置を変更
    owner_->ChangeSocketTransformLinear(0.1f,
        owner_->Weapon_Offset_Aim.pos,
        owner_->Weapon_Offset_Aim.rot
    );

    //射撃
    if (Input::GameSupport::GetShotButton())
    {
        owner_->GunShot();
    }

    owner_->TurnY(owner_->GetCamera()->GetTransform()->GetForward(),30.0f);

    //エイムボタンが押されていないなら遷移
    if (!Input::GameSupport::GetAimButton())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Move));
    }
}

void SoldierState::Aim::Finalize()
{
    //自動振り向きをオンに
    owner_->SetEnableAutoTurn(true);

    //カメラを戻す
    owner_->GetCamera()->ZoomReset(zoomParam.time_);
}

void SoldierState::Jump::Initialize()
{
    //ジャンプアニメーション再生
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Jump));

    landTimer_ = 0.0f;
}

void SoldierState::Jump::Update()
{
    //移動
    owner_->InputMove();

    //ジャンプモーション再生終了時にループモーションへ遷移
    if (owner_->GetAnimator()->GetAnimationFinished())
    {
        owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Fall_Loop));
    }

    //着地
    if (owner_->GetOnGround())
    {
        if (landTimer_ == 0)
        {
            //着地後アニメーション再生
            owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Land));
        }

        //タイマー更新
        landTimer_ += Time::deltaTime_;
    }

    //着地して指定秒数経っているなら移動ステートに遷移
    if (landTimer_ > 0.3f)
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Move));
    }
}

void SoldierState::Jump::Finalize()
{
    

}
