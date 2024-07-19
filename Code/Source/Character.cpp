#include "Character.h"
#include "Engine.h"
#include "Actor.h"
#include "StageManager.h"
#include "SkeletalMesh.h"

#include <algorithm>

#include "imgui/imgui.h"

using namespace AbyssEngine;

void Character::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //マネージャーの登録と初期化
    ScriptComponent::Initialize(actor);
}

bool Character::DrawImGui()
{
    ImGui::DragFloat3("Velocity", &velocity_.x);

    float speed = velocity_.Length();
    ImGui::SliderFloat("Speed",&speed,0.0f,Max_Horizontal_Speed);

    ImGui::DragFloat("Max Horizontal Speed", &Max_Horizontal_Speed,0.1f,0.1f);
    ImGui::DragFloat("Max Vertical Speed", &Max_Vertical_Speed,0.1f,0.1f);

    ImGui::SliderFloat("Accel", &acceleration_, 0.0f, 100.0f);
    ImGui::SliderFloat("Decel", &deceleration_, 0.0f, 100.0f);

    ImGui::SliderFloat("Rot Speed", &baseRotSpeed_, 0.0f, 1000.0f);
    ImGui::SliderFloat("Max Rot Speed", &Max_Rot_Speed,0.0f,1000.0f);
    ImGui::SliderFloat("Min Rot Speed", &Min_Rot_Speed,0.0f,1000.0f);

    return true;
}

void Character::Jump(const float& jumpPower)
{
    //現在空中にいる場合はジャンプしない
    if (!onGround_)return;

    velocity_.y += jumpPower;

    onGround_ = false;
}

void Character::TurnY(Vector3 dir, bool smooth)
{
    Vector2 d = { dir.x,dir.z };

    if (d.LengthSquared() == 0)return;

    //XZ軸のみのベクトル正規化
    d.Normalize();
    dir = { d.x,0,d.y };
    auto forward = transform_->GetForward();

    float rotY = transform_->GetRotation().y;

    //内積値から最終的に向きたい角度を計算する
    float dot = forward.Dot(dir);
    if (dot > 0.999f)return;
    float rotAmount = acosf(dot);

    float rotSpeed = 0;

    //なめらかな振り向きか、即座に振り向くか
    if (smooth)
    {
        //内積値を0~1の範囲に
        dot = dot + 1.0f;

        rotSpeed = baseRotSpeed_ * rotAmount;
        
    }
    else
    {
        rotSpeed = rotAmount;
    }

    //回転速度制限
    std::clamp(rotSpeed, Min_Rot_Speed, Max_Rot_Speed);

    //外積のY軸のみ求め、回転方向を求める
    float crossY = forward.z * dir.x - forward.x * dir.z;
    if (crossY < 0) rotSpeed = -rotSpeed;
    
    rotY += rotSpeed * Time::deltaTime_;
    transform_->SetRotationY(rotY);
}

void Character::Move()
{
    UpdateMove(); 
}

void Character::UpdateVelocity()
{
    //速力更新

    //水平方向
    {
        if (moveVec_.LengthSquared() > 0.01f)
        {
            if (onGround_)
            {
                velocity_ = velocity_ + moveVec_ * (acceleration_ * Time::deltaTime_);
            }
            else
            {
                //空中にいるときは制御しづらくする
                velocity_ = velocity_ + moveVec_ * (acceleration_ * Time::deltaTime_) * airborneCoefficient_;
            }

            //速度制限
            Vector2 velocityXZ = { velocity_.x,velocity_.z };
            if (velocityXZ.Length() > Max_Horizontal_Speed)
            {
                velocityXZ.Normalize();
                velocityXZ = velocityXZ * Max_Horizontal_Speed;

                velocity_.x = velocityXZ.x;
                velocity_.z = velocityXZ.y;
            }
        }
        else//減速処理
        {
            //入力値がほぼない場合は減速処理
            Vector3 velocityXZ = {velocity_.x,0,velocity_.z};
            Vector3 veloXZNormal;
            velocityXZ.Normalize(veloXZNormal);

            if (onGround_)
            {
                velocityXZ = velocity_ - (veloXZNormal * (deceleration_ * Time::deltaTime_));
            }
            else
            {
                velocityXZ = velocity_ - (veloXZNormal * (airResistance_ * Time::deltaTime_));
            }
            velocityXZ.y = 0;

            Vector3 veloNormal;
            velocity_.Normalize(veloNormal);
            velocityXZ.Normalize(veloXZNormal);

            //反対方向のベクトルになってしまうか速度が遅すぎるなら、速度を完全に０にする
            if (veloNormal.Dot(veloXZNormal) < 0 || velocityXZ.LengthSquared() < 0.1f)
            {
                velocity_ = { 0,velocity_.y,0 };
            }
            else
            {
                velocity_ = { velocityXZ.x,velocity_.y,velocityXZ.z };
            }
        }
    }

    //縦方向の速力更新
    {
        //重力による速力更新
        velocity_.y += Gravity * weight_ * Time::deltaTime_;

        //速度制限
        if (fabsf(velocity_.y) > Max_Vertical_Speed)
        {
            velocity_.y = velocity_.y > 0 ? Max_Vertical_Speed : -Max_Vertical_Speed;
        }
    }
}

void Character::UpdateMove()
{
    UpdateVelocity();

    //動くことがない時は処理しない
    //if (velocity_.LengthSquared() * Time::deltaTime_ < 0.01f)return;
    
    UpdateHorizontalMove();
    UpdateVerticalMove();

    //回転
    TurnY(Vector3(velocity_.x,0,velocity_.z));
}

void Character::UpdateHorizontalMove()
{
    if (velocity_.x * velocity_.x + velocity_.z * velocity_.z == 0)return;

    //速度から何の判定もしなかったときの移動後の座標を取得
    const Vector3 pos = transform_->GetPosition();
    const Vector3 moveVector =
    {
        velocity_.x * Time::deltaTime_,
        0,
        velocity_.z * Time::deltaTime_
    };

    Vector3 moved = pos + moveVector;//移動後の座標

    Vector3 hit;//光線がヒットしたところの座標
    Vector3 hitNormal;//光線がヒットした面の法線

    //レイの開始地点引き上げ
    static constexpr float correctionY = 1.0f;//補正値
    Vector3 start = pos;
    start.y += correctionY;
    Vector3 end = moved;
    end.y += correctionY;


    //地形判定
    const auto& stage = StageManager::Instance().GetActiveStage();
    if (stage->RayCast(start, end, hit, hitNormal))
    {
        //スタートからレイが当たった位置までのベクトル
        Vector3 endToHit = hit - end;

        //内積を使い壁ずりベクトルを算出
        Vector3 projection = hitNormal * (hitNormal.Dot(endToHit));

        //壁ずり移動のために必要な数値を用意
        Vector3 startWall = hit + hitNormal * 0.01f;//壁にべったりつきすぎないようにしている
        Vector3 endWall = end + projection + hitNormal * 0.01f;

        //壁ずり移動
        if (stage->RayCast(startWall, endWall, hit, hitNormal))
        {
            //再度当たった場合は壁ずりはしない
            hit.y -= correctionY;
            moved = hit;
        }
        else
        {
            //当たらなかった
            endWall.y -= correctionY;
            moved = endWall;
        }

        //移動距離に応じて速度を強制的に変更
        {
            float moveDistance = Vector3(moved - pos).Length() / Time::deltaTime_;
            velocity_.Normalize();
            velocity_ = velocity_ * moveDistance;
        }
    }
    else
    {
        //当たらなかった
    }

    //座標更新
    transform_->SetPositionX(moved.x);
    transform_->SetPositionZ(moved.z);
}

void Character::UpdateVerticalMove()
{
    //速度から何の判定もしなかったときの移動後の座標を取得
    const Vector3 pos = transform_->GetPosition();
    const float moveY = velocity_.y * Time::deltaTime_;

    //移動後の座標
    Vector3 moved = {pos.x,pos.y + moveY,pos.z};

    Vector3 hit;//光線がヒットしたところの座標
    Vector3 hitNormal;//光線がヒットした面の法線

    //レイの開始地点引き上げ
    static constexpr float correctionY = 1.0f;//補正値
    Vector3 start = pos;
    start.y += correctionY;
    Vector3 end = moved;

    //垂直方向に地形判定
    const auto& stage = StageManager::Instance().GetActiveStage();
    if (stage->RayCast(start,end,hit,hitNormal))
    {
        //着地した
        Landing();
        moved.y = hit.y;
    }

    transform_->SetPosition(moved);
}

void Character::Landing()
{
    velocity_.y = 0.0f;
    onGround_ = true;
}
