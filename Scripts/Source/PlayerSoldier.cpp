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
    //�A�N�^�[�ƃg�����X�t�H�[���̓o�^
    Character::Initialize(actor);

    //���f���ǂݍ���
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

    //���[�g�m�[�h��ݒ�
    for (const auto& node : model_->GetModel()->nodes_)
    {
        if (node.name_ == "root")
        {
            model_->GetAnimator()->SetRootJointIndex(node.index_);
            break;
        }
    }

    //����𑕔�������
    weaponModel_ = actor_->AddComponent<StaticMesh>("./Assets/Models/Soldier/Soldier_Gun.glb");
    model_->SocketAttach(weaponModel_, "middle_metacarpal_l");
    weaponModel_->GetSocketData().location_ = Weapon_Offset_Move.pos;
    weaponModel_->GetSocketData().rotation_ = Weapon_Offset_Move.rot;

    gunComponent_ = actor_->AddComponent<Gun>();
    gunComponent_->SetColliderTag(Collider::Tag::Player);

    //�v���C���[�J�����ݒ�(�v���C���[�Ɛe�q�֌W��)
    //���͂��̂܂܃A�^�b�`���Ă��邪�A��X�Ǝ��̃J��������������
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

    //���̃J���������C����
    camera_->ChangeMainCamera(camera_.get());


    //�X�e�[�g�}�V���ݒ�
    stateMachine_ = actor->AddComponent<StateMachine<State<Soldier>>>();
    stateMachine_->RegisterState(new SoldierState::Move(this));
    stateMachine_->RegisterState(new SoldierState::Aim(this));
    stateMachine_->RegisterState(new SoldierState::Jump(this));
    stateMachine_->RegisterState(new SoldierState::Dodge(this));
    stateMachine_->SetState(static_cast<int>(ActionState::Move));

    //�\�P�b�g���̐ݒ�
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

    //�e���̃I�t�Z�b�g�ʒu�ݒ�
    muzzleOffsetPos_ = { 0.0f,0.16f,0.49f };

    //TODO:���u���̓����蔻��
    const auto& collider = AddHitCollider(center_,0.2f);
    collider->ReplaceTag(Collider::Tag::Player);
}

void Soldier::Update()
{
    MuzzlePosUpdate();

    //�v���C���[�J���������C���ɂȂ��Ă��Ȃ������撆�̓X�e�[�g�}�V���̍X�V�����Ȃ�
    //�X�V���������Ȃ�
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

    //���B�e�X����\��������
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

    //�n�`������܂߂��ړ�����
    Character::UpdateMove();

    //��]
    //TurnY(velocity_);

    const Vector2 veloXZ = { velocity_.x,velocity_.z };
    moveAnimation_->SetBlendWeight(veloXZ.Length() / Run_Max_Speed);

    moveVec_ = {};
}

void Soldier::CameraRollUpdate()
{
    //���͒l�擾
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
    //�I�t�Z�b�g�s��
    const Matrix Offset = Matrix::CreateTranslation(muzzleOffsetPos_);

    //���[���h�s��Z�o
    const Matrix M = Offset * weaponModel_->GetWorldMatrix();

    //�ړ������𒊏o
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

    //�@�̂Ƃ̋������瓋��\���ǂ����𔻒f
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
    //���͂��ꂽ�x�N�g������ړ��x�N�g���擾
    auto inputVec = Input::GameSupport::GetMoveVector();

    //���͒l������ꍇ�͉���
    if (inputVec.LengthSquared() > 0.01f) 
    {
        moveVec_ = camera_->ConvertTo2DVectorFromCamera(inputVec);

        //�X�e�B�b�N�����������|���Ă���Ȃ�A�������ɂ��A�x�����x��ۂ�����
        ///���͍X�V
        {
            //�X�e�B�b�N�̓��͂̑傫���ɉ����ĉ����x��ύX
            acceleration_ = baseAcceleration_ * inputVec.Length();

            velocity_ = velocity_ + moveVec_ * (acceleration_ * Time::deltaTime_);

        }
    }
    else
    {
        moveVec_ = {};
    }

    //�W�����v
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
    //�ˌ�
    //��ʒ����Ƀ��C���΂��A���������ꏊ�Ɍ������Ēe����Ԃ悤�ɂ���
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
        //���������ʒu�ɔ�΂�
        Vector3 toTarget = hitPos - gunComponent_->GetMuzzlePos();
        toTarget.Normalize();
        shootDirection = toTarget;
    }
    else
    {
        //������Ȃ��Ȃ�A�J�����̌�����
        const Vector3 target = start + eyeToFocus * 50.0f;
        Vector3 toTarget = target - gunComponent_->GetMuzzlePos();
        toTarget.Normalize();
        shootDirection = toTarget;
    }

    if (gunComponent_->Shot(shootDirection))
    {
        //��ʐU��
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
    //�����Ă��邩
    if (onGround_)
    {
#if 0
        Max_Horizontal_Speed = Input::GameSupport::GetDashButton() ? Run_Max_Speed : Walk_Max_Speed;
#else
        //�_�b�V���{�^���̉������݊֌W�Ȃ����点�Ă���
        Max_Horizontal_Speed = Run_Max_Speed;
#endif // 0

        
    }
}

bool Soldier::BoardingTheVitesse()
{
    //����\�ȏ�Ԃ�
    if (!canBoarding_)
    {
        return false;
    }
   
    //����
    vitesse_->GetOnBoardPilot(std::static_pointer_cast<Soldier>(shared_from_this()));

    //�v���C���[���f���̕`����~�߂�
    model_->SetEnable(false);

    vitesseOnBoard_ = true;

    return true;
}
