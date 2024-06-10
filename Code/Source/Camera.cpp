#include "Camera.h"
#include "DXSystem.h"
#include "Actor.h"
#include "imgui/imgui.h"
#include "RenderManager.h"
#include "Engine.h"
#include "Input.h"

#include <algorithm>
#include <Windows.h>

using namespace AbyssEngine;
using namespace DirectX;

void Camera::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();

    Engine::renderManager_->Add(static_pointer_cast<Camera>(shared_from_this()));
}

bool Camera::DrawImGui()
{
    if (ImGui::TreeNode("Camera"))
    {
        //�g�p����J������This�ɕύX
        if (ImGui::Button("Use This Camera"))
        {
            RenderManager::ChangeMainCamera(this);
        }

        ImGui::DragFloat("Debug Speed", &debCameraSpeed_, 0.1f, 0.01f);

        ImGui::SliderAngle("Fov", &fov_, 30.0f, 120.0f);
        ImGui::DragFloat("Near Z", &nearZ_, 0.01f, 0.01f,1.0f);
        ImGui::DragFloat("Far Z", &farZ_, 1.0f, 0.1f);

        ImGui::DragFloat("Arm Length", &armLength_, 0.1f, 0.1f);
        ImGui::DragFloat("Camera Lag Speed", &cameraLagSpeed_, 0.1f,0.1f);
        ImGui::DragFloat3("Socket Offset", &socketOffset_.x, 0.1f);
        ImGui::DragFloat3("Target Offset", &targetOffset_.x, 0.1f);

        ImGui::TreePop();
    }

    return true;
}

void Camera::Update()
{
#if _DEBUG
    DebugCameraController();
#endif // _DEBUG

    //�J�����̒x���Ǐ]�X�V
    CameraLagUpdate();

    const float aspect = static_cast<float>(DXSystem::GetScreenWidth())
        / static_cast<float>(DXSystem::GetScreenHeight()); //��ʔ䗦
    projectionMatrix_ = XMMatrixPerspectiveFovLH(fov_, aspect, nearZ_, farZ_);

    //�r���[�s��쐬
    if (viewTarget_)
    {
        focus_ = transform_->GetPosition() + socketOffset_;
        eye_ = focus_ - transform_->GetForward() * armLength_;
    }
    else
    {
        focus_ = transform_->GetPosition();
        eye_ = focus_ - transform_->GetForward();
    }
    const Vector3 up = transform_->GetUp();

    //�e�����x�N�g���̍X�V
    transform_->CalcWorldMatrix();

    viewMatrix_ = XMMatrixLookAtLH(eye_, focus_, up);
    viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

Vector3 Camera::ConvertTo3DVectorFromCamera(const Vector2& v)
{
    Vector3 result;

    const auto& forward = transform_->GetForward();
    const auto& right = transform_->GetForward();

    result.x = forward.x * v.y + right.x * v.x;
    result.y = forward.y * v.y + right.y * v.x;
    result.z = forward.z * v.y + right.z * v.x;

    result.Normalize();

    return result;
}

Vector3 Camera::ConvertTo2DVectorFromCamera(const Vector2& v)
{
    Vector2 result;

    const auto& forward = transform_->GetForward();
    const auto& right = transform_->GetRight();

    result.x = forward.x * v.y + right.x * v.x;
    result.y = forward.z * v.y + right.z * v.x;

    result.Normalize();

    return Vector3(result.x,0,result.y);
}

void Camera::DebugCameraController()
{
    if (!enableDebugController_)return;

    //�}�E�X�A�L�[�{�[�h�ɂ��J��������
    {
        auto rot = transform_->GetRotation();
        //�}�E�X����

        static bool inputStart = false;
        if (Mouse::GetButtonState().rightButton)
        {
            if (inputStart)
            {
                POINT pos{};
                GetCursorPos(&pos);
                auto mouseVec = Vector2(960.0f, 540.0f) - Vector2(static_cast<float>(pos.x), static_cast<float>(pos.y));
                if (mouseVec.x != 0 || mouseVec.y != 0)
                {
                    rot.y -= mouseVec.x * Time::deltaTime_;
                    rot.x -= mouseVec.y * Time::deltaTime_;
                }
            }
            else
            {
                inputStart = true;
            }
            SetCursorPos(960, 540);
        }
        else
        {
            inputStart = false;
        }
        

        //�R���g���[���[����
        {
            auto& gamepad = Input::GetGamePad();
            auto stick = Vector2(gamepad.GetAxisRX(), gamepad.GetAxisRY());
            if (stick.x != 0 || stick.y != 0)
            {
                rot.y += stick.x * Time::deltaTime_;
                rot.x -= stick.y * Time::deltaTime_;
            }
        }

        const float CAMERA_MAX_ROT_X = 150.0f;
        const float CAMERA_MIN_ROT_X = -150.0f;
        rot.x = std::clamp(rot.x, CAMERA_MIN_ROT_X, CAMERA_MAX_ROT_X);

        transform_->SetRotation(rot);
    }

    //�ړ�����
    {
        //�}�E�X�z�C�[���ł̈ړ�
        const float mouseWheel = Mouse::GetScrollWheelValue();

        Vector2 input = {};
        if (Mouse::GetButtonState().rightButton)
        {
            //WASD
            if (Keyboard::GetKeyState().D)input.x += 1.0f;
            if (Keyboard::GetKeyState().A)input.x -= 1.0f;
            if (Keyboard::GetKeyState().W)input.y += 1.0f;
            if (Keyboard::GetKeyState().S)input.y -= 1.0f;
        }
        //���͒l���Ȃ��ꍇ�������Ȃ�
        if (input.Length() > 0 || mouseWheel != 0.0f)
        {
            Vector3 move = {
                transform_->GetForward() * (input.y + mouseWheel * Time::deltaTime_) + transform_->GetRight() * input.x
            };
            move = move * Time::deltaTime_ * debCameraSpeed_;
            auto pos = transform_->GetPosition();
            transform_->SetPosition(pos + move);
        }
    }
}

void Camera::CameraLagUpdate()
{
    if (viewTarget_ == nullptr)return;

    auto cameraPos = transform_->GetPosition();

    //��]���l�������I�t�Z�b�g�ʒu���v�Z
    const auto& offset = transform_->GetRight() * targetOffset_.x + transform_->GetUp() * targetOffset_.y + transform_->GetForward() * targetOffset_.z;
    const auto& target = viewTarget_->GetPosition() + offset;

    //�J��������r���[�^�[�Q�b�g�ւ̃x�N�g��
    const auto& vec = target - cameraPos;


    //�ړ��x�N�g���v�Z
    const auto& moveVec = Vector3::Lerp(Vector3(0,0,0), vec, 1.0f / cameraLagSpeed_) * 100.0f/*�P���ɒl�����������邩�瑝�₵�Ă邾��*/;
    cameraPos = cameraPos + moveVec * Time::deltaTime_;

    transform_->SetPosition(cameraPos);
}
