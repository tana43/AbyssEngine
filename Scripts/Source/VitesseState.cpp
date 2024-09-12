#include "VitesseState.h"
#include "Vitesse.h"
#include "Animator.h"
#include "Easing.h"
#include "Actor.h"
#include "Engine.h"
#include "Input.h"

using namespace AbyssEngine;

void VitesseState::GroundMove::Initialize()
{
    //アニメーション設定
    owner_->ChangeAnimationState(Vitesse::AnimationState::Ground_Move);
}

void VitesseState::GroundMove::Update()
{
    owner_->UpdateInputMove();

    //ジャンプボタンが押された際に離陸ステートへ
    if (Input::GameSupport::GetJumpButton())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Vitesse::ActionState::TakeOff));
    }
}

void VitesseState::GroundMove::Finalize()
{
    owner_->ChangeAnimationState(Vitesse::AnimationState::Default);
}

void VitesseState::FlyMove::Initialize()
{
    //アニメーション設定
    //owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimState::Fly_Move));
    owner_->ChangeAnimationState(Vitesse::AnimationState::Fly_Move);

    //空中移動
}

void VitesseState::FlyMove::Update()
{
    owner_->UpdateInputMove();
    owner_->ThrusterInfluenceVelocity();

    //各ステートに遷移
    if (owner_->GetOnGround())
    {
        //着地しているなら着地ステートへ
        
        owner_->ChangeState(Vitesse::ActionState::Landing);
    }
}

void VitesseState::FlyMove::Finalize()
{
    //アニメステートをデフォルトへ
    owner_->ChangeAnimationState(Vitesse::AnimationState::Default);

    //スラスター全停止
    owner_->ThrusterAllStop();
}

void VitesseState::Landing::Initialize()
{
    //アニメーション設定
    //owner_->ChangeAnimationState(Vitesse::AnimationState::Fly_Move);
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Landing));

    //陸上モードへ移行
    owner_->ToGroundMode();

    //減速させるために移動方向を０に
    owner_->SetMoveVec(Vector3::Zero);

    //減速力をキャッシュ、調整
    cachedDeceleration_ = owner_->GetDeceleration();
    owner_->SetDeceleration(deceleration_);
}

void VitesseState::Landing::Update()
{
    //アニメーションが終了次第GroundMoveへ
    if (owner_->GetAnimator()->GetAnimationFinished())
    {

        owner_->ChangeState(Vitesse::ActionState::GMove);
    }
}

void VitesseState::Landing::Finalize()
{
    //減速力を元に戻す
    owner_->SetDeceleration(cachedDeceleration_);
}


void VitesseState::TakeOff::Initialize()
{
    //アニメーション設定
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Fly_Up));
    startPosition_ = owner_->GetTransform()->GetPosition().y;
    timer_ = 0;

    //フライトモードへ移行
    owner_->ToFlightMode();

    //慣性は消しておく
    owner_->SetVelocityY(0.0f);

    //着地フラグをおろしておく
    owner_->SetOnGround(false);

    //目標高度を設定 
    goalAltitude_ = startPosition_ + altitude_;
}

void VitesseState::TakeOff::Update()
{
    //アニメーションが遷移しきっており、ある程度の時間が経過しているなら空中移動モーションへ
    if (owner_->GetAnimator()->GetNextAnimClip() != static_cast<int>(Vitesse::AnimationIndex::Fly_Move)
        && owner_->GetAnimator()->GetCurrentAnimClip() == static_cast<int>(Vitesse::AnimationIndex::Fly_Up)
        && timer_ > requidTime_ / 4.0f)
    {
        //owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Fly_Move),requidTime_ / 4.0f);
        owner_->ChangeAnimationState(Vitesse::AnimationState::Fly_Move);
    }

    //イージングを使ってY座標を上に動かす
    float positionY = Easing::OutCubic(timer_, requidTime_, goalAltitude_,startPosition_);

    owner_->GetTransform()->SetPositionY(positionY);

    timer_ += Time::deltaTime_;


    //空中へステート遷移
    if (timer_  > requidTime_)
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Vitesse::ActionState::FMove));
    }
}

void VitesseState::TakeOff::Finalize()
{
}
