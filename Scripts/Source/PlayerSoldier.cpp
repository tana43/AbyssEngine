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
    model_ = actor_->AddComponent<SkeletalMesh>("./Assets/Models/Soldier/Sci_Fi_Soldier_03_Idle.glb");
    model_->GetAnimator()->AppendAnimations({
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_WalkFwd.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_RunFwd.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Idle_ADS.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Jump.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Fall_Loop.glb",
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_Land.glb"
                },
        {
            "Walk","Run","Aim","Jump","Fall_Loop","Land"
        });
    AnimBlendSpace1D moveAnim = AnimBlendSpace1D(model_.get(), "Move", 0, 2);
    moveAnim.AddBlendAnimation(1, 0.6f);
    moveAnimation_ = model_->GetAnimator()->AppendAnimation(moveAnim);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Jump)]->SetLoopFlag(false);
    model_->GetAnimator()->GetAnimations()[static_cast<int>(AnimState::Land)]->SetLoopFlag(false);

    model_->GetAnimator()->PlayAnimation(static_cast<int>(AnimState::Move));

    //����𑕔�������
    weaponModel_ = actor_->AddComponent<StaticMesh>("./Assets/Models/Soldier/Soldier_Gun.glb");
    model_->SocketAttach(weaponModel_, "middle_metacarpal_l");
    weaponModel_->GetSocketData().location_ = Weapon_Offset_Move.pos;
    weaponModel_->GetSocketData().rotation_ = Weapon_Offset_Move.rot;

    gunComponent_ = actor_->AddComponent<Gun>();

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


    //�X�e�[�g�}�V���ݒ�
    stateMachine_ = actor->AddComponent<StateMachine<State<Soldier>>>();
    stateMachine_->RegisterState(new SoldierState::Move(this));
    stateMachine_->RegisterState(new SoldierState::Aim(this));
    stateMachine_->RegisterState(new SoldierState::Jump(this));
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

bool Soldier::DrawImGui()
{
    if (ImGui::TreeNode("Player"))
    {
        Character::DrawImGui();

        ImGui::DragFloat("Base Acceleration", &baseAcceleration_, 0.1f, 0.0f, 100.0f);

        ImGui::DragFloat("Jump Power", &jumpPower_, 0.02f, 0.01f, 100.0f);

        stateMachine_->DrawImGui();

        ImGui::TreePop();

    }


    return false;
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
    gunComponent_->SetMuzzlePos(weaponModel_->GetWorldMatrix().Translation());
}

void Soldier::BoardingDistanceJudge(const float& range)
{
    if (!vitesse_)return;

    //�@�̂Ƃ̋������瓋��\���ǂ����𔻒f
    auto myPos = GetTransform()->GetPosition();
    auto viPos = vitesse_->GetTransform()->GetPosition();
    float dist = Vector3::Distance(myPos, viPos);
    if (dist < range)
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
    if (StageManager::Instance().GetActiveStage()->RayCast(
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
        Max_Horizontal_Speed = Input::GameSupport::GetDashButton() ? Run_Max_Speed : Walk_Max_Speed;
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
