#include "CharacterManager.h"
#include "Engine.h"
#include "Actor.h"
#include "StageManager.h"

#include <algorithm>

#include "imgui/imgui.h"

using namespace AbyssEngine;

void Character::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //マネージャーの登録と初期化
    Engine::characterManager_->AddCharacter(std::static_pointer_cast<Character>(shared_from_this()));
    actor_ = actor;
    transform_ = actor->GetTransform();
}

bool Character::DrawImGui()
{
    ImGui::DragFloat3("Velocity", &velocity_.x);

    float speed = velocity_.Length();
    ImGui::SliderFloat("Speed",&speed,0.0f,Max_Speed);

    ImGui::DragFloat("Max Speed", &Max_Speed,0.1f,0.1f);

    ImGui::SliderFloat("Accel", &acceleration_, 0.0f, 10.0f);
    ImGui::SliderFloat("Decel", &deceleration_, 0.0f, 10.0f);

    ImGui::SliderFloat("Rot Speed", &baseRotSpeed_, 0.0f, 1000.0f);
    ImGui::SliderFloat("Max Rot Speed", &Max_Rot_Speed,0.0f,1000.0f);
    ImGui::SliderFloat("Min Rot Speed", &Min_Rot_Speed,0.0f,1000.0f);

    return true;
}

void Character::TurnY(Vector3 dir, bool smooth)
{
    if (dir.LengthSquared() == 0)return;

    //XZ軸のみのベクトル正規化
    Vector2 d = { dir.x,dir.z };
    d.Normalize();
    dir = { d.x,0,d.y };
    auto forward = transform_->GetForward();

    float rotY = transform_->GetRotation().y;

    //内積値から最終的に向きたい角度を計算する
    float dot = forward.Dot(dir);
    if (dot > 0.99f)return;
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
    UpdateVelocity();
    UpdateMove(); 
}

void Character::UpdateVelocity()
{
    //速力更新
    {
        if (moveVec_.LengthSquared() > 0.01f)
        {
            velocity_ = velocity_ + moveVec_ * (acceleration_ * Time::deltaTime_);

            //速度制限
            if (velocity_.Length() > Max_Speed)
            {
                velocity_.Normalize();
                velocity_ = velocity_ * Max_Speed;
            }
        }
        else//減速処理
        {
            //入力値がほぼない場合は減速処理
            velocity_ = velocity_ - (velocity_ * (deceleration_ * Time::deltaTime_));

            //速度が０に近いときは完全に０にする
            if (velocity_.LengthSquared() > 0.01f)
            {
                velocity_ = {};
            }
        }
    }
}

void Character::UpdateMove()
{
    //動くことがない時は処理しない
    if (velocity_.LengthSquared() * Time::deltaTime_ < 0.01f)return;
    
    //速度から何の判定もしなかったときの移動後の座標を取得
    const Vector3 pos = transform_->GetPosition(); 
    const Vector3 moveVector = velocity_ * Time::deltaTime_;
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
    if(stage->RayCast(start,end,hit,hitNormal))
    {
#if 0
        //仮で動かないようにしてみる
        hit.y -= correctionY;
        hit = hit + hitNormal * 0.1f;
        moved = hit;
#else

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
#endif // 0
      
    

    //座標更新
    transform_->SetPosition(moved);

    //回転
    TurnY(velocity_);
}
