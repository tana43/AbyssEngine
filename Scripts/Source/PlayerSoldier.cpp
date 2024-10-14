#include "PlayerSoldier.h"
#include "Engine.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SkeletalMesh.h"
#include "StaticMesh.h"
#include "Camera.h"
#include "Actor.h"
#include "Input.h"
#include "MathHelper.h"
#include "Vitesse.h"
#include "Gun.h"
#include "PlayerSoldierState.h"
#include "BillboardRenderer.h"
#include "SphereCollider.h"
#include "GameCollider.h"

#include "StageManager.h"

#include "Animator.h"
#include "Animation.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;

void Soldier::Initialize(const std::shared_ptr<Actor>& actor)
{
    //アクターとトランスフォームの登録
    Character::Initialize(actor);

    //モデル読み込み
#if 1
    model_ = actor_->AddComponent<SkeletalMesh>("./Assets/Models/Soldier/Sci_Fi_Soldier_03_Idle.gltf");
    model_->GetAnimator()->AppendAnimations({
                //"./Assets/Models/Soldier/Sci_Fi_Soldier_03_Idle.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_WalkFwd.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_RunFwd.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Idle_ADS.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Jump.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Fall_Loop.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Land.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_Roll_Back.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_Roll_Forward.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_Roll_Right.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_Roll_Left.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_Cartwheel_Back.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_Cartwheel_Forward.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_NoHandSpin_Right.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_NoHandSpin_Left.glb"
        },
        {
            //"Idle",
            "Walk",
            "Run",
            "Aim",
            "Jump",
            "Fall_Loop",
            "Land",
            "Dodge_Roll_Back",
            "Dodge_Roll_Fwd",
            "Dodge_Roll_Right",
            "Dodge_Roll_Left",
            "Cartwheel_Back",
            "Cartwheel_Forward",
            "NoHandSpin_Right",
            "NoHandSpin_Left"
        });
#else
    model_ = actor_->AddComponent<SkeletalMesh>("./Assets/Models/Soldier/Sci_Fi_Soldier_03_Idle.glb");
    model_->GetAnimator()->AppendAnimations({
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_WalkFwd.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_RunFwd.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Idle_ADS.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Jump.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Fall_Loop.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Land.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_Roll_Back.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_Roll_Forward.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_Roll_Right.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_Roll_Left.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_Cartwheel_Back.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_Cartwheel_Forward.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_NoHandSpin_Right.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Dodge_NoHandSpin_Left.glb"
        },
        {
            "Walk",
            "Run",
            "Aim",
            "Jump",
            "Fall_Loop",
            "Land",
            "Dodge_Roll_Back",
            "Dodge_Roll_Fwd",
            "Dodge_Roll_Right",
            "Dodge_Roll_Left",
            "Cartwheel_Back",
            "Cartwheel_Forward",
            "NoHandSpin_Right",
            "NoHandSpin_Left"
        });
#endif // 0

    
    AnimBlendSpace1D moveAnim = AnimBlendSpace1D(model_.get(), "Move", 0, 2);
    moveAnim.AddBlendAnimation(1, 0.6f);
    moveAnimation_ = model_->GetAnimator()->AppendAnimation(moveAnim);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Jump)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Land)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Dodge_Back)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Dodge_Fwd)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Dodge_Right)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Dodge_Left)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Cartwheel_Back)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Cartwheel_Forward)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::NoHandSpin_Right)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::NoHandSpin_Left)]->SetLoopFlag(false);

    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Dodge_Back)]->SetRootMotion(true);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Dodge_Fwd)]->SetRootMotion(true);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Dodge_Right)]->SetRootMotion(true);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Dodge_Left)]->SetRootMotion(true);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Cartwheel_Back)]->SetRootMotion(true);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Cartwheel_Forward)]->SetRootMotion(true);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::NoHandSpin_Right)]->SetRootMotion(true);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::NoHandSpin_Left)]->SetRootMotion(true);

    float dodgeAnimSpeed = 1.3f;
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Dodge_Back)]->SetAnimSpeed(dodgeAnimSpeed);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Dodge_Fwd)]->SetAnimSpeed(dodgeAnimSpeed);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Dodge_Right)]->SetAnimSpeed(dodgeAnimSpeed);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Dodge_Left)]->SetAnimSpeed(dodgeAnimSpeed);
    //model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Cartwheel_Back)]->SetAnimSpeed(dodgeAnimSpeed);
    //model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Cartwheel_Forward)]->SetAnimSpeed(dodgeAnimSpeed);
    //model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::NoHandSpin_Right)]->SetAnimSpeed(dodgeAnimSpeed);
    //model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::NoHandSpin_Left)]->SetAnimSpeed(dodgeAnimSpeed);

    float rootMotionDodgeSpeed = 2.0f;
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Cartwheel_Back)]->SetRootMotionSpeed(rootMotionDodgeSpeed);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Cartwheel_Forward)]->SetRootMotionSpeed(rootMotionDodgeSpeed);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::NoHandSpin_Right)]->SetRootMotionSpeed(rootMotionDodgeSpeed);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::NoHandSpin_Left)]->SetRootMotionSpeed(rootMotionDodgeSpeed);

    model_->GetAnimator()->PlayAnimation(static_cast<int>(AnimState::Move));

    //ルートノードを設定
    for (const auto& node : model_->GetModel()->nodes_)
    {
        if (node.name_ == "root")
        {
            model_->GetAnimator()->SetRootJointIndex(node.index_);
            break;
        }
    }

    //武器を装備させる
    weaponModel_ = actor_->AddComponent<StaticMesh>("./Assets/Models/Soldier/Soldier_Gun.glb");
    model_->SocketAttach(weaponModel_, "middle_metacarpal_l");
    weaponModel_->GetSocketData().location_ = Weapon_Offset_Move.pos;
    weaponModel_->GetSocketData().rotation_ = Weapon_Offset_Move.rot;

    gunComponent_ = actor_->AddComponent<Gun>();
    gunComponent_->SetColliderTag(Collider::Tag::Player);

    //プレイヤーカメラ設定(プレイヤーと親子関係に)
    //今はそのままアタッチしているが、後々独自のカメラ挙動をつくる
    const auto& c = Engine::sceneManager_->GetActiveScene().InstanceActor("Player Camera");
    camera_ = c->AddComponent<Camera>();
    //c->SetParent(actor_);
    camera_->SetFov(DirectX::XMConvertToRadians(80.0f));
    camera_->SetBaseTargetOffset(Vector3(0.29f, 0.6f, 0));
    camera_->SetBaseArmLength(0.4f);
    camera_->SetTargetOffset(Vector3(0.29f, 0.6f, 0));
    camera_->SetArmLength(0.4f);
    camera_->SetCameraLagSpeed(0.05f);
    camera_->SetViewTarget(transform_.get());
    camera_->SetEnableDebugController(false);

    //このカメラをメインに
    camera_->ChangeMainCamera(camera_.get());


    //ステートマシン設定
    stateMachine_ = actor->AddComponent<StateMachine<State<Soldier>>>();
    stateMachine_->RegisterState(new SoldierState::Move(this));
    stateMachine_->RegisterState(new SoldierState::Aim(this));
    stateMachine_->RegisterState(new SoldierState::Jump(this));
    stateMachine_->RegisterState(new SoldierState::Dodge(this));
    stateMachine_->SetState(static_cast<int>(ActionState::Move));

    //ソケット情報の設定
    float* params[] =
    {
        &weaponModel_->GetSocketData().location_.x,
        &weaponModel_->GetSocketData().location_.y,
        &weaponModel_->GetSocketData().location_.z,
        &weaponModel_->GetSocketData().rotation_.x,
        &weaponModel_->GetSocketData().rotation_.y,
        &weaponModel_->GetSocketData().rotation_.z,
    };
    socketFade_ = std::make_unique<FadeSystem>(params[0], static_cast<size_t>(sizeof(params) / sizeof(float*)));

#if _DEBUG
    camera_->SetEnableDebugController(true);
#endif // _DEBUG

    //銃口のオフセット位置設定
    muzzleOffsetPos_ = { 0.0f,0.16f,0.49f };

    //TODO:仮置きの当たり判定
    const auto& collider = AddHitCollider(center_,0.2f);
    collider->ReplaceTag(Collider::Tag::Player);
}

void Soldier::Update()
{
    MuzzlePosUpdate();

    //プレイヤーカメラがメインになっていないかつ搭乗中はステートマシンの更新をしない
    //更新処理をしない
    if (camera_->GetIsMainCamera() && !vitesseOnBoard_)
    {
        stateMachine_->SetActive(true);
    }
    else
    {
        stateMachine_->SetActive(false);
    }

    MoveUpdate();

    CameraRollUpdate();

    //ヴィテス搭乗可能距離判定
    BoardingDistanceJudge(boardingDistance_);
    
}

void Soldier::DrawImGui()
{
    if (ImGui::TreeNode("Player"))
    {
        Character::DrawImGui();

        ImGui::DragFloat("Base Acceleration", &baseAcceleration_, 0.1f, 0.0f, 100.0f);

        ImGui::DragFloat("Jump Power", &jumpPower_, 0.02f, 0.01f, 100.0f);

        ImGui::DragFloat3("Muzzle Offset", &muzzleOffsetPos_.x, 0.01f);

        stateMachine_->DrawImGui();

        ImGui::TreePop();

    }

}

void Soldier::MoveUpdate()
{
   
    //Character::UpdateVelocity();

    //地形判定も含めた移動処理
    Character::UpdateMove();

    //回転
    //TurnY(velocity_);

    const Vector2 veloXZ = { velocity_.x,velocity_.z };
    moveAnimation_->SetBlendWeight(veloXZ.Length() / Run_Max_Speed);

    moveVec_ = {};
}

void Soldier::CameraRollUpdate()
{
    //入力値取得
    Vector2 input = Input::GameSupport::GetCameraRollVector();

    auto r = camera_->GetTransform()->GetRotation();
    const float rollSpeed = cameraRollSpeed_ * Time::deltaTime_;
    r.x = r.x + input.y * rollSpeed;
    r.y = r.y + input.x * rollSpeed;
    camera_->GetTransform()->SetRotation(r);
}

void Soldier::SocketUpdate()
{
    
}

void Soldier::MuzzlePosUpdate()
{
    //オフセット行列
    const Matrix Offset = Matrix::CreateTranslation(muzzleOffsetPos_);

    //ワールド行列算出
    const Matrix M = Offset * weaponModel_->GetWorldMatrix();

    //移動成分を抽出
    const Vector3 pos = M.Translation();

    gunComponent_->SetMuzzlePos(pos);
}

void Soldier::BoardingDistanceJudge(const float& range)
{
    if (!vitesse_)return;

    if (!vitesse_->GetCanBoarding())
    {
        canBoarding_ = false;
        return;
    }

    //機体との距離から搭乗可能かどうかを判断
    const Vector3 myPos = GetTransform()->GetPosition();
    const Vector3 viPos = vitesse_->GetTransform()->GetPosition();
    float distSq = Vector3::DistanceSquared(myPos, viPos);
    if (distSq < range * range)
    {
        canBoarding_ = true;
    }
    else
    {
        canBoarding_ = false;
    }
}

void Soldier::InputMove()
{
    //入力されたベクトルから移動ベクトル取得
    auto inputVec = Input::GameSupport::GetMoveVector();

    //入力値がある場合は加速
    if (inputVec.LengthSquared() > 0.01f) 
    {
        moveVec_ = camera_->ConvertTo2DVectorFromCamera(inputVec);

        //スティックを少しだけ倒しているなら、加速も緩く、遅い速度を保たせる
        ///速力更新
        {
            //スティックの入力の大きさに応じて加速度を変更
            acceleration_ = baseAcceleration_ * inputVec.Length();

            velocity_ = velocity_ + moveVec_ * (acceleration_ * Time::deltaTime_);

        }
    }
    else
    {
        moveVec_ = {};
    }

    //ジャンプ
    if (canJump_ && Input::GameSupport::GetJumpButton())
    {
        if (Jump(jumpPower_))
        {
            stateMachine_->ChangeState(static_cast<int>(ActionState::Jump));
        }
    }
}

void Soldier::ChangeSocketTransformLinear(const float& changeTime, const AbyssEngine::Vector3& pos, const AbyssEngine::Vector3& rot)
{
    float params[] =
    {
        pos.x,
        pos.y,
        pos.z,
        rot.x,
        rot.y,
        rot.z,
    };
    std::vector<float> f;
    for (size_t i = 0; i < 6; i++)
    {
        f.emplace_back(params[i]);
    }
    socketFade_->Fade(changeTime, f);
}

void Soldier::GunShot()
{
    //射撃
    //画面中央にレイを飛ばし、当たった場所に向かって弾が飛ぶようにする
    Vector3 eyeToFocus = camera_->GetFocus() - camera_->GetEye();
    eyeToFocus.Normalize();
    const float range = 1000.0f;

    const Vector3 start = camera_->GetEye();
    const Vector3 end = start + eyeToFocus * range;
    Vector3 hitPos,hitNormal,shootDirection;
    const auto& stage = Engine::stageManager_->GetActiveStage().lock();
    if (stage->RayCast(
        start,end,hitPos,hitNormal
    ))
    {
        //当たった位置に飛ばす
        Vector3 toTarget = hitPos - gunComponent_->GetMuzzlePos();
        toTarget.Normalize();
        shootDirection = toTarget;
    }
    else
    {
        //当たらないなら、カメラの向きへ
        const Vector3 target = start + eyeToFocus * 50.0f;
        Vector3 toTarget = target - gunComponent_->GetMuzzlePos();
        toTarget.Normalize();
        shootDirection = toTarget;
    }

    if (gunComponent_->Shot(shootDirection))
    {
        //画面振動
        Camera::CameraShakeParameters param;
        param.position_.amplitudeMultiplier_ = 0.03f;
        param.position_.frequencyMultiplier_ = 5.0f;
        param.rotation_.amplitudeMultiplier_ = 0.0f;
        param.timing_.duration_ = 0.05f;
        param.timing_.blendOutTime_ = 0.05f;
        camera_->CameraShake(param);
    }
}

void Soldier::DashDecision()
{
    //走っているか
    if (onGround_)
    {
#if 0
        Max_Horizontal_Speed = Input::GameSupport::GetDashButton() ? Run_Max_Speed : Walk_Max_Speed;
#else
        //ダッシュボタンの押し込み関係なく走らせている
        Max_Horizontal_Speed = Run_Max_Speed;
#endif // 0

        
    }
}

bool Soldier::BoardingTheVitesse()
{
    //搭乗可能な状態か
    if (!canBoarding_)
    {
        return false;
    }
   
    //搭乗
    vitesse_->GetOnBoardPilot(std::static_pointer_cast<Soldier>(shared_from_this()));

    //プレイヤーモデルの描画を止める
    model_->SetEnable(false);

    vitesseOnBoard_ = true;

    return true;
}
