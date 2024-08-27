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

    //ジャンプを可能に
    owner_->SetCanJump(true);
}

void SoldierState::Move::Update()
{
    //ダッシュ判定
    owner_->DashDecision();

    //移動
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

    //回避
    if (Input::GameSupport::GetDashButton())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Dodge));
    }
}

void SoldierState::Move::Finalize()
{

}

void SoldierState::Aim::Initialize()
{
    timer_ = 0.0f;

    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Aim));

    //自動振り向きをオフに
    owner_->SetEnableAutoTurn(false);

    //カメラを寄らせる
    owner_->GetCamera()->Zoom(zoomParam);

    //ジャンプを不可に
    owner_->SetCanJump(false);

    //歩行速度を遅くする
    owner_->SetMaxHorizontalSpeed(Max_Move_Speed);

}

void SoldierState::Aim::Update()
{
    //時間更新
    timer_ += Time::deltaTime_;

    //武器の位置を変更
    owner_->ChangeSocketTransformLinear(0.1f,
        owner_->Weapon_Offset_Aim.pos,
        owner_->Weapon_Offset_Aim.rot
    );

    //移動
    owner_->InputMove();

    //射撃
    if (Input::GameSupport::GetShotButton())
    {
        owner_->GunShot();
    }

    owner_->TurnY(owner_->GetCamera()->GetTransform()->GetForward(),30.0f);

    //エイムボタンが押されていないかつ、時間がしばらく経っていたなら遷移
    if (!Input::GameSupport::GetAimButton() && timer_ > Minimum_Time)
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

    //ジャンプを可能に
    owner_->SetCanJump(true);
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

void SoldierState::Dodge::Initialize()
{
    //左スティック
    Vector2 ax = Input::GameSupport::GetMoveVector();
    if (ax.LengthSquared() == 0)
    {
        Vector3 f = owner_->GetTransform()->GetForward();
        f.Normalize();
        ax = { f.x,f.z };
    }
    Vector3 moveVec = owner_->GetCamera()->ConvertTo2DVectorFromCamera(ax);
    Direction dir = DirectionJudge(moveVec);

    //回避アニメーション再生
    switch (dir)
    {
    case SoldierState::Dodge::Direction::Back:
        owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Back));
        break;
    case SoldierState::Dodge::Direction::Forward:
        owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Fwd));
        break;
    case SoldierState::Dodge::Direction::Right:
        owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Right));
        break;
    case SoldierState::Dodge::Direction::Left:
        owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Left));
        break;
    }

    timer_ = 0;

    secondDodge_ = false;
}

void SoldierState::Dodge::Update()
{
    if (!secondDodge_)
    {
        if (timer_ > Cancel_Time)
        {
            if (Input::GameSupport::GetDodgeButton())
            {
                Vector2 ax = Input::GameSupport::GetMoveVector();
                if (ax.LengthSquared() == 0)
                {
                    Vector3 f = owner_->GetTransform()->GetForward();
                    f.Normalize();
                    ax = { f.x,f.z };
                }
                Vector3 moveVec = owner_->GetCamera()->ConvertTo2DVectorFromCamera(ax);
                Direction dir = DirectionJudge(moveVec);

                //回避アニメーション再生
                float transTime = 0.3f;//遷移時間
                switch (dir)
                {
                case SoldierState::Dodge::Direction::Back:
                    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Cartwheel_Back), transTime);
                    break;
                case SoldierState::Dodge::Direction::Forward:
                    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Cartwheel_Forward), transTime);
                    break;
                case SoldierState::Dodge::Direction::Right:
                    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::NoHandSpin_Right), transTime);
                    break;
                case SoldierState::Dodge::Direction::Left:
                    owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::NoHandSpin_Left), transTime);
                    break;
                }

                //タイマーリセット
                //timer_ = 0;

                secondDodge_ = true;
            }
        }
    }

    //ステートの切り替え
    if (owner_->GetAnimator()->GetAnimationFinished())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Move));
    }

    //経過時間
    timer_ += Time::deltaTime_;
}

void SoldierState::Dodge::Finalize()
{
}

SoldierState::Dodge::Direction SoldierState::Dodge::DirectionJudge(const Vector3& moveVec)
{
    float dot = moveVec.Dot(owner_->GetTransform()->GetForward());
    float degree = DirectX::XMConvertToDegrees(acosf(dot));
    
    //方向を算出
    Direction dir = Direction::Forward;
    if (degree < 45.0f)
    {
        dir = Direction::Forward;
    }
    else if(degree < 135.0f)
    {
        //左右判定
        if (moveVec.x > 0)
        {
            dir = Direction::Right;
        }
        else
        {
            dir = Direction::Left;
        }
    }
    else
    {
        dir = Direction::Back;
    }

    return dir;
}
