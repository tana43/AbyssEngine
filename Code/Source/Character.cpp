#include "Character.h"
#include "Engine.h"
#include "Actor.h"
#include "StageManager.h"
#include "SkeletalMesh.h"

#include <algorithm>

#include "imgui/imgui.h"
#include "Easing.h"

using namespace AbyssEngine;

void Character::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //マネージャーの登録と初期化
    ScriptComponent::Initialize(actor);
}

void Character::Update()
{
    UpdateHitStop();

    UpdateImpactMove();

    UpdateMove();
}

void Character::DrawImGui()
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

    ImGui::DragFloat3("Move Vec", &moveVec_.x, 0.05f, -1.0f, 1.0f);

    ImGui::DragFloat("Terrain Radius", &terrainRadius_, 0.01f, 0.0f);
    ImGui::DragFloat("Terrain Center Offset", &terrainCenterOffset_, 0.01f, 0.0f);
    ImGui::DragFloat("Terrain Step Offset", &terrainStepOffset_, 0.01f, 0.0f);

    ImGui::DragFloat("Max_Health", &Max_Health, 0.1f);
    ImGui::SliderFloat("Health", &health_, 0.0f,Max_Health);

    ImGui::DragFloat("Gravity", &Gravity,0.01f);

    static float hitStopDuration = 1.0f;
    static float hitStopOutTime = 0.5f;
    if (ImGui::TreeNode("HitStop"))
    {
        ImGui::DragFloat("HitStop Duration", &hitStopDuration,0.02f);
        ImGui::SliderFloat("HitStop OutTime", &hitStopOutTime,0.0f,hitStopDuration);

        if (ImGui::Button("Play HitStop"))
        {
            HitStop(hitStopDuration, hitStopOutTime);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Impulse"))
    {
        static Vector3 vec;
        ImGui::DragFloat("Impact Deccel", &inpactDeccel_);
        ImGui::DragFloat("Impact Speed Max", &inpactSpeedMax_);
        ImGui::DragFloat3("vector", &vec.x);

        if (ImGui::Button("Add Impulse"))
        {
            AddImpulse(vec);
        }

        ImGui::TreePop();
    }
}

bool Character::Jump(const float& jumpPower)
{
    //現在空中にいる場合はジャンプしない
    if (!onGround_)return false;

    velocity_.y += jumpPower;

    onGround_ = false;

    return true;
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
    float rotAmount = acosf(dot);
    if (dot > 0.999f)
    {
        return;
    }

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

    if (smooth)
    {
        rotY += rotSpeed * actor_->GetDeltaTime();
    }
    else
    {
        rotY += rotSpeed;
    }
    transform_->SetRotationY(rotY);
}

void AbyssEngine::Character::TurnY(Vector3 dir, const float& speed, bool smooth)
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
    float rotAmount = DirectX::XMConvertToDegrees(acosf(dot));

    float rotSpeed = 0;

    //なめらかな振り向きか、即座に振り向くか
    if (smooth)
    {
        //内積値を0~1の範囲に
        dot = dot + 1.0f;

        rotSpeed = speed * rotAmount * actor_->GetDeltaTime();

        if (rotSpeed > rotAmount)
        {
            rotSpeed = rotAmount;
        }
    }
    else
    {
        rotSpeed = rotAmount;
    }

    //外積のY軸のみ求め、回転方向を求める
    float crossY = forward.z * dir.x - forward.x * dir.z;
    if (crossY < 0) rotSpeed = -rotSpeed;
    
    rotY += rotSpeed;
    
    transform_->SetRotationY(rotY);
}

bool Character::ApplyDamage(const AttackParameter& param, DamageResult* damageResult)
{
    //無敵か
    if (invincible_)
    {
        if (damageResult)*damageResult = DamageResult::Failed;
        return false;
    }

    //既に死んでいるか
    if (health_ < 0)
    {
        if (damageResult)*damageResult = DamageResult::Failed;
        return false;
    }

    //ダメージ処理
    health_ -= param.power_;

    //死亡処理
    if (health_ < 0)
    {
        health_ = 0;
        Die();
        if (damageResult)*damageResult = DamageResult::FinalBlow;
    }
    else
    {
        if (damageResult)*damageResult = DamageResult::Success;
    }

    return true;
}

void Character::Die()
{
    //actor_->Destroy(actor_);
    isDead_ = true;
}

void AbyssEngine::Character::HitStop(float duration, float blendOutTime)
{
    hitStopDuration_ = duration;
    hitStopOutTime_ = blendOutTime;
    hitStopTimer_ = 0;
}

void AbyssEngine::Character::AddImpulse(Vector3 impulse)
{
    impactMove_ = impactMove_ + impulse;
}

Vector3 Character::GetCenterPos()
{
    return transform_->GetPosition() + center_;
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
                velocity_ = velocity_ + moveVec_ * (acceleration_ * actor_->GetDeltaTime());
            }
            else
            {
                //空中にいるときは制御しづらくする
                velocity_ = velocity_ + moveVec_ * (acceleration_ * actor_->GetDeltaTime()) * airborneCoefficient_;
            }

            //速度制限
            //速度をある程度越えている場合、減速させていく
            Vector2 velocityXZ = { velocity_.x,velocity_.z };
            float spd = velocityXZ.Length();
            if (spd > Max_Horizontal_Speed)
            {
                velocityXZ.Normalize();
                if (spd - Max_Horizontal_Speed < 0.1f)
                {
                    //そのまま最大速度を代入
                    velocityXZ = velocityXZ * Max_Horizontal_Speed;
                }
                else
                {
                    //減速させる
                    float newSpeed = spd - speedingDecel_ * actor_->GetDeltaTime();
                    velocityXZ = velocityXZ * newSpeed;
                }

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
                velocityXZ = velocity_ - (veloXZNormal * (deceleration_ * actor_->GetDeltaTime()));
            }
            else
            {
                velocityXZ = velocity_ - (veloXZNormal * (airResistance_ * actor_->GetDeltaTime()));
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
        if (enableGravity_)
        {
            //重力による速力更新
            velocity_.y += Gravity * weight_ * actor_->GetDeltaTime();
        }

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
    //if (velocity_.LengthSquared() * actor_->GetDeltaTime() < 0.01f)return;
    
    UpdateHorizontalMove();
    UpdateVerticalMove();

    //回転
    if (enableAutoTurn_)
    {
        TurnY(Vector3(velocity_.x,0,velocity_.z));
    }
}

void Character::UpdateHorizontalMove()
{
    //if (velocity_.x * velocity_.x + velocity_.z * velocity_.z == 0)return;

    //壁衝突フラグをリセット
    hitWall_ = false;

    //純粋な移動後の座標を取得
    const Vector3 pos = transform_->GetPosition();
    Vector3 move =
    {
        velocity_.x * actor_->GetDeltaTime(),
        0,
        velocity_.z * actor_->GetDeltaTime()
    };

    //アニメーターを持っているならルートモーションによる移動値を加算
    if (const auto& animator = actor_->GetComponent<Animator>())
    {
        Vector3 rootMotion = animator->GetRootMotionMove();
        move.x += rootMotion.x;
        move.z += rootMotion.z;
        animator->SetRootMotionMove(Vector3(0, rootMotion.y, 0));
    }

    const Vector3 moveVector = move;

    //押し出し処理による移動値も反映させる
    const Vector3 externalMove = { externalFactorsMove_.x,0,externalFactorsMove_.z };

    Vector3 moved = pos + moveVector + externalMove;//移動後の座標

    //スフィアキャスト
    if (sphereCast_)
    {
        //中心点
        Vector3 centerOffset = { 0,terrainRadius_ + terrainCenterOffset_,0 };

        // キャスト量を算出
        float distance = moveVector.Length();
        if (distance > 0.000001f)
        {
            //  壁より少し手前で止まってほしいので、キャスト量を増やす(skinWidth)
            distance += terrainSkinWidth_;

            Vector3 origin = transform_->GetPosition() + centerOffset;
            Vector3 direction = moveVector.Normalize();
            Vector3 hitPosition, hitNormal;

            //スフィアキャスト
            const auto& stage = Engine::stageManager_->GetActiveStage().lock();
            if (stage && stage->SphereCast(
                origin, direction, terrainRadius_, distance, hitPosition, hitNormal, true))
            {
                //  キャスト量を増やした分だけ減らす(skinWidth)
                distance -= terrainSkinWidth_;

                //移動
                moved = pos + direction * distance;

                //滑り処理
                {
                    //  次回移動が壁に沿うよう移動ベクトルを算出
                    float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(hitNormal, moveVector));
                    const Vector3 move = moveVector - hitNormal * dot;
                    moved = pos + move;
                }

                //壁に当たっているか
                if (hitNormal.Dot(Vector3(0, 1, 0)) > 0.8f)
                {
                    hitWall_ = true;
                }
            }
        }
    }
    //レイキャストver
    else
    {
        Vector3 hit;//光線がヒットしたところの座標
        Vector3 hitNormal;//光線がヒットした面の法線

        //レイの開始地点引き上げ
        static constexpr float correctionY = 0.5f;//補正値
        Vector3 start = pos;
        start.y += correctionY;
        Vector3 end = moved;
        end.y += correctionY;


        //地形判定
        const auto& stage = Engine::stageManager_->GetActiveStage().lock();
        if (stage.get() && stage->RayCast(start, end, hit, hitNormal))
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
                float moveDistance = Vector3(moved - pos).Length() / actor_->GetDeltaTime();
                velocity_.Normalize();
                velocity_ = velocity_ * moveDistance;
            }

            //壁に当たった
            hitWall_ = true;
        }
        else
        {
            //当たらなかった
        }
    }

    //座標更新
    transform_->SetPositionX(moved.x);
    transform_->SetPositionZ(moved.z);
}

void Character::UpdateVerticalMove()
{
    //速度から何の判定もしなかったときの移動後の座標を取得
    const Vector3 pos = transform_->GetPosition();
    float moveY = velocity_.y * actor_->GetDeltaTime();

    //アニメーターを持っているならルートモーションによる移動値を加算
    if (const auto& animator = actor_->GetComponent<Animator>())
    {
        Vector3 rootMotion = animator->GetRootMotionMove();
        moveY += rootMotion.y;
        animator->SetRootMotionMove(Vector3(rootMotion.x, 0, rootMotion.z));

    }

    //移動後の座標 外的要因による移動値も計算する
    Vector3 moved = {pos.x,pos.y + moveY + externalFactorsMove_.y,pos.z};

    //スフィアキャスト
    if (sphereCast_)
    {
        //中心点
        Vector3 centerOffset = { 0,terrainRadius_ + terrainStepOffset_,0 };

        // キャスト量を算出
        float distance = fabsf(moveY);
        if (distance > 0.000001f)
        {
            //オフセット分加算
            distance += terrainStepOffset_;

            Vector3 origin = transform_->GetPosition() + centerOffset;
            Vector3 direction = velocity_.y > 0 ? Vector3(0, 1, 0) : Vector3(0, -1, 0);
            Vector3 hitPosition, hitNormal;

            //スフィアキャスト
            const auto& stage = Engine::stageManager_->GetActiveStage().lock();
            if (stage && stage->SphereCast(
                origin,direction,terrainRadius_,distance,hitPosition,hitNormal,true))
            {
                //スロープ角度を算出
                DirectX::XMVECTOR Up = DirectX::XMVectorSet(0, 1, 0, 0);
                float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Up, hitNormal));
                float angle = DirectX::XMConvertToDegrees(acosf(dot));

                //着地した
                Landing();
                distance -= terrainStepOffset_;
                moved.y = pos.y + direction.y * distance;

                //  制限角度以内なら滑る処理はスキップ
                if (angle > slopeLimit)
                {
                    Vector3 move = { 0, moveY, 0 };

                    //  次回移動が壁に沿うよう移動ベクトルを算出
                    float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(hitNormal,move));
                    move = DirectX::XMVectorSubtract(move, DirectX::XMVectorScale(hitNormal, dot));
                    moved = pos + move;
                }
            }
            else
            {
                onGround_ = false;
            }
        }
    }
    //レイキャスト
    else
    {
        Vector3 hit;//光線がヒットしたところの座標
        Vector3 hitNormal;//光線がヒットした面の法線

        //レイの開始地点引き上げ
        float correctionY = terrainStepOffset_;//補正値
        Vector3 start = pos;
        start.y += correctionY;
        Vector3 end = moved;

        //垂直方向に地形判定
        const auto& stage = Engine::stageManager_->GetActiveStage().lock();
        if (stage && stage->RayCast(start, end, hit, hitNormal))
        {
            //着地した
            Landing();
            moved.y = hit.y;
        }
    }

    transform_->SetPosition(moved);

    externalFactorsMove_ = Vector3::Zero;
}

void Character::Landing()
{
    velocity_.y = 0.0f;
    onGround_ = true;
}

void Character::UpdateImpactMove()
{
    //現在の衝撃移動値を減らしていく
    float length = impactMove_.Length();
    if (length < 0.0001f) 
    {
        impactMove_ = Vector3::Zero;
        return;
    }

    //速度が大きいほど減速するようにする
    float decel = length / inpactDeccel_;
    decel *= decel;
    if (decel > inpactSpeedMax_)decel = inpactSpeedMax_;

    decel = decel * actor_->GetDeltaTime();
    float speed = length - decel;
    if (speed > 0)
    {
        impactMove_.Normalize();
        impactMove_ = impactMove_ * speed;
    }
    else
    {
        impactMove_ = Vector3::Zero;
    }

    //移動値を加算する
    AddExternalFactorsMove(impactMove_ * actor_->GetDeltaTime());
}

void Character::UpdateHitStop()
{
    if (hitStopTimer_ > hitStopDuration_)
    {
        actor_->SetTimeScale(1.0f);
        return;
    }

    //ヒットストップ処理
    //フェードが必要か
    if (hitStopTimer_ > hitStopOutTime_)
    {
        float duration = hitStopDuration_ - hitStopOutTime_;
        float time = hitStopTimer_ - hitStopOutTime_;
        float weight = time / duration;

        actor_->SetTimeScale(Easing::InCubic(weight, 1.0f, 1.0f, 0.0f));
    }
    else
    {
        //停止させる
        actor_->SetTimeScale(0.0f);
    }

    //ワールドの経過時間で加算していく
    hitStopTimer_ += Time::GetDeltaTime();
}
