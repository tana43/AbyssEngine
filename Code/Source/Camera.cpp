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
        ImGui::SliderAngle("Fov", &fov_, 30.0f, 120.0f);
        ImGui::DragFloat("Near Z", &nearZ_, 0.01f, 0.01f,1.0f);
        ImGui::DragFloat("Far Z", &farZ_, 1.0f, 0.1f);

        ImGui::TreePop();
    }

    return true;
}

void Camera::Update()
{
#if _DEBUG
    DebugCameraController();
#endif // _DEBUG

    const float aspect = static_cast<float>(DXSystem::GetScreenWidth()) 
        / static_cast<float>(DXSystem::GetScreenHeight()); //画面比率
    projectionMatrix_ = XMMatrixPerspectiveFovLH(fov_, aspect, nearZ_, farZ_);

    //ビュー行列作成
    const Vector3 eye = transform_->GetPosition();
    const Vector3 focus = eye + transform_->GetForward();
    const Vector3 up = transform_->GetUp();

    //各方向ベクトルの更新
    transform_->CalcWorldMatrix();

    viewMatrix_ = XMMatrixLookAtLH(eye, focus, up);
    viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

void Camera::DebugCameraController()
{
    //マウス、キーボードによるカメラ操作
    {
        auto rot = transform_->GetRotation();
        //マウス操作

        static bool inputStart = false;
        if (Mouse::GetButtonState().rightButton)
        {
            if (inputStart)
            {
                POINT pos{};
                GetCursorPos(&pos);
                auto mouseVec = Vector2(960, 540) - Vector2(pos.x, pos.y);
                if (mouseVec.x != 0 || mouseVec.y != 0)
                {
                    rot.y -= mouseVec.x * 0.05f * Time::deltaTime_;
                    rot.x -= mouseVec.y * 0.05f * Time::deltaTime_;
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

        //コントローラー操作
        {
            auto& gamepad = Input::GetGamePad();
            auto stick = Vector2(gamepad.GetAxisRX(), gamepad.GetAxisRY());
            if (stick.x != 0 || stick.y != 0)
            {
                rot.y += stick.x * Time::deltaTime_;
                rot.x -= stick.y * Time::deltaTime_;
            }
        }

        const float CAMERA_MAX_ROT_X = 1.309f;
        const float CAMERA_MIN_ROT_X = -1.309f;
        rot.x = std::clamp(rot.x, CAMERA_MIN_ROT_X, CAMERA_MAX_ROT_X);

        transform_->SetRotation(rot);
    }

    //移動処理
    {
        //WASD
        Vector2 input = {};
        if (Keyboard::GetKeyState().D)input.x += 1.0f;
        if (Keyboard::GetKeyState().A)input.x -= 1.0f;
        if (Keyboard::GetKeyState().W)input.y += 1.0f;
        if (Keyboard::GetKeyState().S)input.y -= 1.0f;

        //入力値がない場合処理しない
        if (input.Length() > 0)
        {
            const Vector3 move = {
                transform_->GetForward() * input.y + transform_->GetRight() * input.x
            };
            auto pos = transform_->GetPosition();
            transform_->SetPosition(pos + move);
        }
    }
}
