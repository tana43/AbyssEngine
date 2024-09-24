#include "VitesseState.h"
#include "Vitesse.h"
#include "Animator.h"
#include "Easing.h"
#include "Actor.h"
#include "Engine.h"
#include "Input.h"
#include "RenderManager.h"

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

    //誰も乗っていないなら乗り込みステートへ
    if (!owner_->GetPilot().lock())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Vitesse::ActionState::Boarding));
    }

    //ダッシュボタンが押されているなら高速ステートへ
    if (Input::GameSupport::GetDashButton())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Vitesse::ActionState::HighSpeedFlight));
    }
}

void VitesseState::GroundMove::Finalize()
{
    owner_->ChangeAnimationState(Vitesse::AnimationState::Default);
}

void VitesseState::Flight::Initialize()
{
    //アニメーション設定
    //owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimState::Fly_Move));
    owner_->ChangeAnimationState(Vitesse::AnimationState::Flight_Move);

    //空中移動
}

void VitesseState::Flight::Update()
{
    owner_->UpdateInputMove();
    owner_->ThrusterInfluenceVelocity();

    //各ステートに遷移
    
    //着地しているなら着地ステートへ
    if (owner_->GetOnGround())
    {    
        owner_->ChangeState(Vitesse::ActionState::Landing);
    }

    //ダッシュボタンが押されているなら高速ステートへ
    if (Input::GameSupport::GetDashButton())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Vitesse::ActionState::HighSpeedFlight));
    }
}

void VitesseState::Flight::Finalize()
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
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Flight_Up));
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
    if (owner_->GetAnimator()->GetNextAnimClip() != static_cast<int>(Vitesse::AnimationIndex::Flight_Move)
        && owner_->GetAnimator()->GetCurrentAnimClip() == static_cast<int>(Vitesse::AnimationIndex::Flight_Up)
        && timer_ > requidTime_ / 4.0f)
    {
        //owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Fly_Move),requidTime_ / 4.0f);
        owner_->ChangeAnimationState(Vitesse::AnimationState::Flight_Move);
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

void VitesseState::Boarding::Initialize()
{
    //アニメーション設定
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Board_Standby));

    //乗り込み可能な状態へ
    owner_->SetCanBoarding(true);

    board_ = false;
}

void VitesseState::Boarding::Update()
{
    //乗り込んでいないとき
    //アニメーションが終了しており、プレイヤーが乗り込んでいるなら移動ステートへ遷移
    if (!board_)
    {
        if (owner_->GetAnimator()->GetAnimationFinished())
        {
            if (owner_->GetPilot().lock())
            {
                owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Board_Complete));
                board_ = true;
            }
        }
    }
    //乗り込んだなら、アニメーションの終了と同時にステートを遷移
    else
    {
        if (owner_->GetAnimator()->GetAnimationFinished())
        {
            owner_->GetStateMachine()->ChangeState(static_cast<int>(Vitesse::ActionState::GMove));
        }
    }
}

void VitesseState::Boarding::Finalize()
{
    //アニメーション設定
    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Board_Complete));

    //乗り込み不可へ
    owner_->SetCanBoarding(false);
}

void VitesseState::HighSpeedFlight::Initialize()
{
    rollingDodge_ = false;

    //回避(入力値が無い場合、前方向へ回避)
    auto inputVec = Input::GameSupport::GetMoveVector();
    auto moveVec = owner_->GetCamera()->ConvertTo3DVectorFromCamera(inputVec);
    if (moveVec.LengthSquared() < 0.1f)moveVec = owner_->GetTransform()->GetForward();
    owner_->Dodge(moveVec);
    owner_->SetMoveVec(moveVec);

    //アニメーション設定
    //斜め入力のときは、専用モーションへ
    inputVec.Normalize();
    const float radius = acosf(inputVec.Dot(Vector2(0, 1)));
    //しきい値設定
    const float rollingDodgeArea[2] = { DirectX::XMConvertToRadians(30),DirectX::XMConvertToRadians(60) };
    const float rollingDodgeAreaBack[2] = { DirectX::XMConvertToRadians(120),DirectX::XMConvertToRadians(150) };
    if ((radius > rollingDodgeArea[0] && radius < rollingDodgeArea[1]) || 
        (radius > rollingDodgeAreaBack[0] && radius < rollingDodgeAreaBack[1]))
    {
        if (inputVec.x > 0)
        {
            owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Dodge_FR));
        }
        else
        {
            owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Dodge_FR));
        }
        rollingDodge_ = true;
    }
    else
    {
        owner_->ChangeAnimationState(Vitesse::AnimationState::HighSpeedFlight);
    }

    //一度離陸させてから高速移動へ
    //フライトモードへ移行
    owner_->ToFlightMode();
    owner_->SetOnGround(false);

    
    //最大速度を変更
    owner_->SetMaxHorizontalSpeed(owner_->GetDodgeMaxSpeed());
    owner_->SetMaxVerticalSpeed(owner_->GetDodgeMaxSpeed());

    //ラジアルブラー設定
    auto& postEffect = Engine::renderManager_->GetBufferEffects().data_;
    postEffect.radialBlurStrength_ = 0.3f;
    postEffect.radialBlurSampleCount_ = 3;

    //カメラのズームを変更
    auto& camera = owner_->GetCamera();
    Camera::ZoomParam zoomParam;
    zoomParam.armLength_ = dodgeCameraArmLength_;
    zoomParam.time_ = 0.05f;
    zoomParam.socketOffset_ = camera->GetSocketOffset();
    zoomParam.targetOffset_ = camera->GetTargetOffset();
    camera->Zoom(zoomParam);
    camera->SetCameraLagSpeed(cameraLagSpeed);

    timer_ = 0.0f;
}

void VitesseState::HighSpeedFlight::Update()
{
    //スラスター噴射
    owner_->ThrusterInfluenceVelocity();

    //移動方向の設定
    owner_->UpdateInputMove();
    //何も入力がないときは前方向へ動かす
    if (owner_->GetMoveVec().LengthSquared() < 0.1f)
    {
        owner_->SetMoveVec(owner_->GetCamera()->ConvertTo3DVectorFromCamera(Vector2(0,1)));
    }

    //ローリング回避終了判定
    if (rollingDodge_)
    {
        if (owner_->GetAnimator()->GetAnimationFinished())
        {
            rollingDodge_ = false;

            //通常回避移動へ
            //回転するモーションである都合上、一旦高速移動のモーションを挟む必要がある
            owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::HighSpeedFlight_F),0.0f);
            owner_->ChangeAnimationState(Vitesse::AnimationState::HighSpeedFlight);
        }
    }

    //カメラアームを調整
    float blendWeight = fminf(timer_ / dodgeTime_,1.0f);
    float armLemgth = std::lerp(dodgeCameraArmLength_, highSpeedCameraArmLength_, blendWeight);
    owner_->GetCamera()->SetArmLength(armLemgth);

    //回避終了
    if (timer_ > dodgeTime_ && !rollingDodge_)
    {
        //最大速度を高速移動時の値に変更
        owner_->SetMaxHorizontalSpeed(owner_->GetHighSpeedFlightMaxSpeed());
        owner_->SetMaxVerticalSpeed(owner_->GetHighSpeedFlightMaxSpeed());
    }

    //ラジアルブラー設定
    auto& postEffect = Engine::renderManager_->GetBufferEffects().data_;
    const float blurStrength = 0.07f;
    if (postEffect.radialBlurStrength_ > blurStrength)
    {
        postEffect.radialBlurStrength_ -= Time::deltaTime_;
    }
    else
    {
        postEffect.radialBlurStrength_ = blurStrength;
    }

    timer_ += Time::deltaTime_;

    //各ステートに遷移

    //回避中は通常移動へ移行しない
    if (timer_ > dodgeTime_ && !rollingDodge_)
    {
        //ダッシュボタンが押されていないなら通常飛行ステートへ
        if (!Input::GameSupport::GetDashButton())
        {
            owner_->ChangeState(Vitesse::ActionState::FMove);
            return;
        }
    }

    //着地しているなら着地ステートへ
    /*if (owner_->GetOnGround())
    {
        owner_->ChangeState(Vitesse::ActionState::Landing);
        return;
    }*/
}

void VitesseState::HighSpeedFlight::Finalize()
{
    //アニメーションステートをデフォルトに
    owner_->ChangeAnimationState(Vitesse::AnimationState::Default);

    //最大速度を元に戻す
    owner_->SetMaxHorizontalSpeed(owner_->GetDefaultMaxHorizontalSpeed());
    owner_->SetMaxVerticalSpeed(owner_->GetDefaultMaxVerticalSpeed());

    //ラジアルブラー初期化
    auto& postEffect = Engine::renderManager_->GetBufferEffects().data_;
    postEffect.radialBlurStrength_ = 0.0f;
    postEffect.radialBlurSampleCount_ = 1;

    //ズームをリセット
    owner_->GetCamera()->ZoomReset(1.5f);
    owner_->GetCamera()->SetCameraLagSpeed(owner_->GetDefaultCameraLagSpeed());

    if (owner_->GetAnimator()->GetCurrentAnimClip() == static_cast<int>(Vitesse::AnimationIndex::Dodge_FR) ||
        owner_->GetAnimator()->GetCurrentAnimClip() == static_cast<int>(Vitesse::AnimationIndex::Dodge_FL))
    {
        owner_->GetAnimator()->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::HighSpeedFlight_F),0.0f);
    }
}
