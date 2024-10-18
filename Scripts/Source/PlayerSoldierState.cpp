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

void SoldierState::Move::Update(float deltaTime)
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

void SoldierState::Aim::Update(float deltaTime)
{
    //時間更新
    timer_ += deltaTime;

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

void SoldierState::Jump::Update(float deltaTime)
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
        landTimer_ += deltaTime;
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
    PlayDodgeAnimation(dir, false);

    timer_ = 0;

    secondDodge_ = false;
}

void SoldierState::Dodge::Update(float deltaTime)
{
    if (!secondDodge_)
    {
        //ローリング中は自由に旋回させる
        bool canTurn = true;

        Vector2 ax = Input::GameSupport::GetMoveVector();
        if (ax.LengthSquared() == 0)
        {
            Vector3 f = owner_->GetTransform()->GetForward();
            f.Normalize();
            ax = { f.x,f.z };

            //入力値がないときは旋回しない
            canTurn = false;
        }
        Vector3 moveVec = owner_->GetCamera()->ConvertTo2DVectorFromCamera(ax);

        //現在の再生モーションに合わせて旋回
        if (canTurn)
        {
            Vector3 turnVec = moveVec;

            //TODO:回避方向補正　ちょっと別のことさせておくれ
            //回避方向に合わせて旋回方向を調整
            
            //回転方向を調整するための方向
            Vector3 forward = {0,0,1};
            Vector3 right = {1,0,0};

            switch (currentDirection_)
            {
            case SoldierState::Dodge::Direction::Back:
                //foward = ↓
                forward = { 0, 0,-1 };
                right   = {-1, 0, 0 };
                break;
            case SoldierState::Dodge::Direction::Forward:
                //foward = ↑　そのまま

                break;
            case SoldierState::Dodge::Direction::Right:
                //foward = →
                forward = { 1, 0, 0 };
                right   = { 0, 0,-1 };
                break;
            case SoldierState::Dodge::Direction::Left:
                //foward = ←
                forward = {-1, 0, 0 };
                right   = { 0, 0, 1 };
                break;
            default:
                break;
            }

            turnVec = {
                    forward.x * moveVec.z + right.x * moveVec.x,
                    0,
                    forward.z * moveVec.z + right.z * moveVec.x };

            owner_->TurnY(turnVec, 200.0f * deltaTime, true);
        }

        //側転回避への遷移
        if (timer_ > Dodge_Cancel_Time)
        {
            if (Input::GameSupport::GetDodgeButton())
            {
                
                Direction dir = DirectionJudge(moveVec);

                PlayDodgeAnimation(dir,true);

                //タイマーリセット
                timer_ = 0;

                secondDodge_ = true;
            }
        }
    }

    //武器の位置を変更
    owner_->ChangeSocketTransformLinear(0.1f,
        owner_->Weapon_Offset_Move.pos,
        owner_->Weapon_Offset_Move.rot
    );

    //ステートの切り替え
    if (owner_->GetAnimator()->GetAnimationFinished())
    {
        owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Move));
    }

    //キャンセル行動
    //現在キャンセル可能か
    bool otherCancel = false;
    bool moveCancel = false;
    if (secondDodge_)
    {
        if (timer_ > Other_Cancel_Time[1])otherCancel = true;
        if (timer_ > Move_Cancel_Time[1])moveCancel = true;
    }
    else
    {
        if (timer_ > Other_Cancel_Time[0])otherCancel = true;
        if (timer_ > Move_Cancel_Time[0])moveCancel = true;
    }



    if (otherCancel)
    {
        //エイムボタンが押されているなら遷移
        if (Input::GameSupport::GetAimButton())
        {
            owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Aim));
        }
    }

    if (moveCancel)
    {
        //移動ステートへ
        if (Input::GameSupport::GetMoveVector().LengthSquared() > 0.1f)
        {
            owner_->GetStateMachine()->ChangeState(static_cast<int>(Soldier::ActionState::Move));
        }
    }

    //経過時間
    timer_ += deltaTime;
}

void SoldierState::Dodge::Finalize()
{
}

SoldierState::Dodge::Direction SoldierState::Dodge::DirectionJudge(const Vector3& moveVec)
{
    const Vector3& forward = owner_->GetTransform()->GetForward();
    float dot = moveVec.Dot(forward);
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
        float crossY = moveVec.z * forward.x - moveVec.x * forward.z;
        if (crossY > 0)
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

void SoldierState::Dodge::PlayDodgeAnimation(Direction dir, bool second)
{
    //回避アニメーション再生
    float transTime = 0.1f;//遷移時間
    float startTime = 0.15f;//何秒目からのモーションを再生させるか
    switch (dir)
    {
    case SoldierState::Dodge::Direction::Back:
        if(second)owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Cartwheel_Back), &transTime, startTime);
        else owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Back));
        break;
    case SoldierState::Dodge::Direction::Forward:
        if (second)owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Cartwheel_Forward), &transTime, startTime);
        else owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Fwd));
        break;
    case SoldierState::Dodge::Direction::Right:
        if (second)owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::NoHandSpin_Right), &transTime, startTime);
        else owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Right));
        break;
    case SoldierState::Dodge::Direction::Left:
        if (second)owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::NoHandSpin_Left), &transTime, startTime);
        else owner_->GetAnimator()->PlayAnimation(static_cast<int>(Soldier::AnimState::Dodge_Left));
        break;
    }
    currentDirection_ = dir;
}

void SoldierState::Die::Initialize()
{
}

void SoldierState::Die::Update(float deltaTime)
{
}

void SoldierState::Die::Finalize()
{
}
