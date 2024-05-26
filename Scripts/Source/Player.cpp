#include "Player.h"
#include "Engine.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SkeletalMesh.h"
#include "Camera.h"
#include "Actor.h"
#include "Input.h"
#include "MathHelper.h"
#include "imgui/imgui.h"

using namespace AbyssEngine;

void Player::Initialize(const std::shared_ptr<Actor>& actor)
{
    //�A�N�^�[�ƃg�����X�t�H�[���̓o�^
    Character::Initialize(actor);

    transform_->SetPositionY(3.17f);

    //���f���ǂݍ���
    model_ = actor_->AddComponent<SkeletalMesh>("./Assets/Models/UE/Manny/Manny_Idle.glb");
    model_->AppendAnimations({
                "./Assets/Models/UE/Manny/Manny_Walk.glb",
                "./Assets/Models/UE/Manny/Manny_Run.glb"
                });

    //�v���C���[�J�����ݒ�(�v���C���[�Ɛe�q�֌W��)
    //���͂��̂܂܃A�^�b�`���Ă��邪�A��X�Ǝ��̃J��������������
    const auto& c = Engine::sceneManager_->GetActiveScene().InstanceActor("Player Camera");
    camera_ = c->AddComponent<Camera>();
    //c->SetParent(actor_);
    camera_->fov_ = DirectX::XMConvertToRadians(80.0f);
    camera_->targetOffset_ = Vector3(0.8f, 1.4f, 0);
    camera_->SetViewTarget(transform_.get());
    camera_->SetEnableDebugController(false);
}

void Player::Update()
{
    MoveUpdate();
}

bool Player::DrawImGui()
{
    if (ImGui::TreeNode("Player"))
    {
        Character::DrawImGui();

        ImGui::DragFloat("Base Acceleration", &baseAcceleration_, 0.1f, 0.0f, 100.0f);

        ImGui::TreePop();
    }

    return false;
}

void Player::MoveUpdate()
{
    //���͂��ꂽ�x�N�g������ړ��x�N�g���擾
    auto inputVec = Input::GetMoveVector();

    //���͒l������ꍇ�͉���
    if (inputVec.LengthSquared() > 0.01f)
    {
        auto moveVec = camera_->ConvertTo2DVectorFromCamera(inputVec);

        //�X�e�B�b�N�����������|���Ă���Ȃ�A�������ɂ��A�x�����x��ۂ�����
        ///���͍X�V
        {
            //�X�e�B�b�N�̓��͂̑傫���ɉ����ĉ����x��ύX
            acceleration_ = baseAcceleration_ * inputVec.Length();

            velocity_ = velocity_ + moveVec * (acceleration_ * Time::deltaTime_);

            //���x����
            if (velocity_.Length() > Max_Speed)
            {
                velocity_.Normalize();
                velocity_ =  velocity_ * Max_Speed;
            }
        }
    }
    else
    {
        //���͒l���قڂȂ��ꍇ�͌�������
        velocity_ = velocity_ - (velocity_ * (deceleration_ * Time::deltaTime_));
    }

    //���W�X�V
    {
        auto pos = transform_->GetPosition();
        pos = pos + velocity_ * Time::deltaTime_;
        transform_->SetPosition(pos);
    }

    //��]
    TurnY(velocity_);

    //�����Ă��邩
    Max_Speed = Input::GetDashButton() ? Run_Max_Speed : Walk_Max_Speed;

}
