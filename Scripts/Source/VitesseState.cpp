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
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimState::Run_Move));
}

void VitesseState::GroundMove::Update()
{
    //ジャンプボタンが押された際に離陸ステートへ
    if (Input::GameSupport::GetJumpButton())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Vitesse::ActionState::TakeOff));
    }
}

void VitesseState::GroundMove::Finalize()
{
}

void VitesseState::FlyMove::Initialize()
{
    //アニメーション設定
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimState::Fly_Move));

    //空中移動
}

void VitesseState::FlyMove::Update()
{
    owner_->ThrusterInfluenceVelocity();
}

void VitesseState::FlyMove::Finalize()
{
    owner_->ThrusterAllStop();
}

void VitesseState::Landing::Initialize()
{
    //アニメーション設定
    //owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimState::));
}

void VitesseState::Landing::Update()
{
}

void VitesseState::Landing::Finalize()
{
}


void VitesseState::TakeOff::Initialize()
{
    //アニメーション設定
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimState::Fly_Up));
    startPosition_ = owner_->GetTransform()->GetPosition().y;
    timer_ = 0;

    //フライトモードへ移行
    owner_->ToFlightMode();

    //慣性は消しておく
    owner_->SetVelocityY(0.0f);

    //目標高度を設定 
    goalAltitude_ = startPosition_ + altitude_;
}

void VitesseState::TakeOff::Update()
{
    //アニメーションが遷移しきっており、ある程度の時間が経過しているなら空中移動モーションへ
    if (owner_->GetAnimator()->GetNextAnimClip() != static_cast<int>(Vitesse::AnimState::Fly_Move)
        && owner_->GetAnimator()->GetCurrentAnimClip() == static_cast<int>(Vitesse::AnimState::Fly_Up)
        && timer_ > requidTime_ / 4.0f)
    {
        owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimState::Fly_Move),requidTime_ / 4.0f);
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
