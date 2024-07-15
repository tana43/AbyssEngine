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
                "./Assets/Models/Soldier/Sci_Fi_Soldier_03_RunFwd.glb"
                },
        {
            "Walk","Run"
        });
    AnimBlendSpace1D moveAnim = AnimBlendSpace1D(model_.get(), "Move", 0, 2);
    moveAnim.AddBlendAnimation(1, 0.6f);
    moveAnimation_ = model_->GetAnimator()->AppendAnimation(moveAnim);

    model_->GetAnimator()->PlayAnimation(static_cast<int>(AnimState::Move));

    //����𑕔�������
    weaponModel_ = actor_->AddComponent<StaticMesh>("./Assets/Models/Soldier/Soldier_Gun.glb");
    model_->SocketAttach(weaponModel_, "middle_metacarpal_l");
    weaponModel_->GetSocketData().location_ = { 3.35f,-4.75f,6.3f };
    weaponModel_->GetSocketData().rotation_ = { 2.15f,168.7f,-99.25f };

    //�v���C���[�J�����ݒ�(�v���C���[�Ɛe�q�֌W��)
    //���͂��̂܂܃A�^�b�`���Ă��邪�A��X�Ǝ��̃J��������������
    const auto& c = Engine::sceneManager_->GetActiveScene().InstanceActor("Player Camera");
    camera_ = c->AddComponent<Camera>();
    //c->SetParent(actor_);
    camera_->fov_ = DirectX::XMConvertToRadians(80.0f);
    camera_->targetOffset_ = Vector3(0.4f, 0.6f, 0);
    camera_->armLength_ = 0.4f;
    camera_->cameraLagSpeed_ = 0.05f;
    camera_->SetViewTarget(transform_.get());
    camera_->SetEnableDebugController(false);
}

void Soldier::Update()
{
    //�v���C���[�J���������C���ɂȂ��Ă��Ȃ���΍X�V���Ȃ�
    if (!camera_->GetIsMainCamera())return;

    MoveUpdate();

    CameraRollUpdate();
}

bool Soldier::DrawImGui()
{
    if (ImGui::TreeNode("Player"))
    {
        Character::DrawImGui();

        ImGui::DragFloat("Base Acceleration", &baseAcceleration_, 0.1f, 0.0f, 100.0f);

        ImGui::DragFloat("Jump Power", &jumpPower_, 0.02f, 0.01f, 100.0f);

        ImGui::TreePop();
    }

    return false;
}

void Soldier::MoveUpdate()
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

    //Character::UpdateVelocity();

    //�n�`������܂߂��ړ�����
    Character::UpdateMove();

    //��]
    //TurnY(velocity_);

    //�W�����v
    if (Input::GameSupport::GetJumpButton())
    {
        Jump(jumpPower_);
    }

    //�����Ă��邩
    if (onGround_)
    {
        Max_Horizontal_Speed = Input::GameSupport::GetDashButton() ? Run_Max_Speed : Walk_Max_Speed;
    }

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

void Soldier::BoardingTheVitesse()
{

}
