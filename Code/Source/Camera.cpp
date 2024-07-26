#include "Camera.h"
#include "DXSystem.h"
#include "Actor.h"
#include "imgui/imgui.h"
#include "RenderManager.h"
#include "Engine.h"
#include "Input.h"
#include "LineRenderer.h"
#include "Easing.h"

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
        //使用するカメラをThisに変更
        if (ImGui::Button("Use This Camera"))
        {
            RenderManager::ChangeMainCamera(this);
        }

        ImGui::DragFloat("Debug Speed", &debCameraSpeed_, 0.1f, 0.01f);

        ImGui::SliderAngle("Fov", &fov_, 30.0f, 120.0f);
        ImGui::DragFloat("Near Z", &nearZ_, 0.01f, 0.01f,1.0f);
        ImGui::DragFloat("Far Z", &farZ_, 1.0f, 0.1f);

        ImGui::DragFloat("Arm Length", &armLength_, 0.01f, 0.1f);
        ImGui::DragFloat("Camera Lag Speed", &cameraLagSpeed_, 0.01f,0.001f);
        ImGui::DragFloat3("Socket Offset", &socketOffset_.x, 0.01f);
        ImGui::DragFloat3("Target Offset", &targetOffset_.x, 0.01f);

        ImGui::SliderFloat("Mouse Sensitivity Side ",&mouseSensitivity_.y, 0.5f, 5.0f);
        ImGui::SliderFloat("Mouse Sensitivity Vertical",&mouseSensitivity_.x, 0.5f, 5.0f);

        ImGui::TreePop();

        if (ImGui::TreeNode("CameraShake"))
        {
            static CameraShakeParameters shakeP;

            ImGui::DragFloat3("ShakePos", &shakePosition_.x, 0.1f);
            ImGui::DragFloat3("ShakeRot", &shakeRotation_.x, 0.01f);

            if (ImGui::Button("StartCameraShake"))
            {
                CameraShake(shakeP);
            }

            if (ImGui::TreeNode("Position"))
            {
                ImGui::DragFloat("PosAmplitudeMult", &shakeP.position_.amplitudeMultiplier_, 0.1f);
                ImGui::DragFloat("PosFrequencMult", &shakeP.position_.frequencyMultiplier_, 0.1f);

                ImGui::DragFloat3("PosAmplitudeMult", &shakeP.position_.amplitude_.x, 0.1f);
                ImGui::DragFloat3("PosFrequencMult", &shakeP.position_.frequency_.x, 0.1f);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Rotation"))
            {
                ImGui::DragFloat("RotAmplitudeMult", &shakeP.rotation_.amplitudeMultiplier_, 0.1f);
                ImGui::DragFloat("RotFrequencMult", &shakeP.rotation_.frequencyMultiplier_, 0.1f);

                ImGui::DragFloat3("RotAmplitudeMult", &shakeP.rotation_.amplitude_.x, 0.1f);
                ImGui::DragFloat3("RotFrequencMult", &shakeP.rotation_.frequency_.x, 0.1f);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Timing"))
            {
                ImGui::DragFloat("Duration", &shakeP.timing_.duration_, 0.1f);
                ImGui::DragFloat("BlendInTime", &shakeP.timing_.blendInTime_, 0.05f);
                ImGui::DragFloat("BlendOutTime", &shakeP.timing_.blendOutTime_, 0.05f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    return true;
}

void Camera::Update()
{
#if _DEBUG
    DebugCameraController();
#endif // _DEBUG

    //カメラの遅延追従更新
    CameraLagUpdate();

    //カメラシェイク更新
    CameraShakeUpdate();

    //各方向ベクトルの更新
    const auto& worldMatrix = transform_->CalcWorldMatrix();

    const float aspect = static_cast<float>(DXSystem::GetScreenWidth())
        / static_cast<float>(DXSystem::GetScreenHeight()); //画面比率
    projectionMatrix_ = XMMatrixPerspectiveFovLH(fov_, aspect, nearZ_, farZ_);


    //カメラシェイクを反映させた各方向ベクトルを算出
    const Vector4 rot = transform_->GetRotation() + shakeRotation_;
    const auto q = Quaternion::Euler(rot.x,rot.y,rot.z);
    const auto R = Matrix::CreateFromQuaternion(q);
    const Vector3 forward = {R._31,R._32,R._33};
    const Vector3 right = { R._11,R._12,R._13 };
    const Vector3 up = { R._21,R._22,R._23 };

    //ビュー行列作成
    if (viewTarget_)
    {
        //const auto& offset = transform_->GetRight() * targetOffset_.x + transform_->GetUp() * targetOffset_.y + transform_->GetForward() * targetOffset_.z;
        auto offset = right * targetOffset_.x + forward * targetOffset_.z;
        offset.y = targetOffset_.y;

        focus_ = transform_->GetPosition() + socketOffset_ + offset;
        //focus_.y = focus_.y + (transform_->GetForward() * armLength_).y;
        eye_ = focus_ - (forward * armLength_);
    }
    else
    {
        focus_ = transform_->GetPosition();
        eye_ = focus_ - forward;
    }

    //カメラシェイクを反映
    eye_ += shakePosition_;

    viewMatrix_ = XMMatrixLookAtLH(eye_, focus_, up);
    viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

Vector3 Camera::ConvertTo3DVectorFromCamera(const Vector2& v)
{
    Vector3 result;

    const auto& forward = transform_->GetForward();
    const auto& right = transform_->GetRight();

    result.x = forward.x * v.y + right.x * v.x;
    result.y = forward.y * v.y + right.y * v.x;
    result.z = forward.z * v.y + right.z * v.x;

    result.Normalize();

    return result;
}

void Camera::UpdateFrustum()
{
    //初回時のみフラスタム生成
    if (initialFrustum_.Near == 0)//Nearが０のときは生成されていないとみなす
    {
        const float aspect = static_cast<float>(DXSystem::GetScreenWidth())
            / static_cast<float>(DXSystem::GetScreenHeight()); //画面比率
        projectionMatrix_ = XMMatrixPerspectiveFovLH(fov_, aspect, nearZ_, farZ_);
        DirectX::BoundingFrustum::CreateFromMatrix(initialFrustum_, projectionMatrix_);
    }

    //フラスタム更新
    Vector4 rotation = transform_->GetRotation();
    Vector3 euler = { rotation.x,rotation.y,rotation.z };
    initialFrustum_.Transform(frustum_, 1.0f, Quaternion::Euler(euler), eye_);

    //デバッグ表示
#if _DEBUG
    Vector3 corners[8];
    frustum_.GetCorners(corners);

    auto& renderer = Engine::renderManager_->lineRenderer_;
    //手前
    {
        //上
        renderer->AddVertex(corners[0], DirectX::XMFLOAT4(1, 1, 1, 1));
        renderer->AddVertex(corners[1], DirectX::XMFLOAT4(1, 1, 1, 1));

        //右
        renderer->AddVertex(corners[1], DirectX::XMFLOAT4(1, 1, 1, 1));
        renderer->AddVertex(corners[2], DirectX::XMFLOAT4(1, 1, 1, 1));

        //下
        renderer->AddVertex(corners[2], DirectX::XMFLOAT4(1, 1, 1, 1));
        renderer->AddVertex(corners[3], DirectX::XMFLOAT4(1, 1, 1, 1));

        //左
        renderer->AddVertex(corners[3], DirectX::XMFLOAT4(1, 1, 1, 1));
        renderer->AddVertex(corners[0], DirectX::XMFLOAT4(1, 1, 1, 1));
    }

    //奥
    {
        //上
        renderer->AddVertex(corners[4], DirectX::XMFLOAT4(1, 1, 1, 1));
        renderer->AddVertex(corners[5], DirectX::XMFLOAT4(1, 1, 1, 1));

        //右
        renderer->AddVertex(corners[5], DirectX::XMFLOAT4(1, 1, 1, 1));
        renderer->AddVertex(corners[6], DirectX::XMFLOAT4(1, 1, 1, 1));

        //下
        renderer->AddVertex(corners[6], DirectX::XMFLOAT4(1, 1, 1, 1));
        renderer->AddVertex(corners[7], DirectX::XMFLOAT4(1, 1, 1, 1));

        //左
        renderer->AddVertex(corners[7], DirectX::XMFLOAT4(1, 1, 1, 1));
        renderer->AddVertex(corners[4], DirectX::XMFLOAT4(1, 1, 1, 1));
    }

    //上下左右
    {
        //上
        renderer->AddVertex(corners[0], DirectX::XMFLOAT4(1, 1, 1, 1));
        renderer->AddVertex(corners[4], DirectX::XMFLOAT4(1, 1, 1, 1));

        //右
        renderer->AddVertex(corners[1], DirectX::XMFLOAT4(1, 1, 1, 1));
        renderer->AddVertex(corners[5], DirectX::XMFLOAT4(1, 1, 1, 1));

        //下
        renderer->AddVertex(corners[2], DirectX::XMFLOAT4(1, 1, 1, 1));
        renderer->AddVertex(corners[6], DirectX::XMFLOAT4(1, 1, 1, 1));

        //左
        renderer->AddVertex(corners[3], DirectX::XMFLOAT4(1, 1, 1, 1));
        renderer->AddVertex(corners[7], DirectX::XMFLOAT4(1, 1, 1, 1));
    }
#endif // _DEBUG
}

void Camera::ChangeMainCamera(Camera* camera)
{
    for (auto& c : Engine::renderManager_->GetCameraList())
    {
        if (const auto& camera = c.lock())
        {
            camera->SetIsMainCamera(false);
        }
    }

    camera->SetIsMainCamera(true);
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

void Camera::Zoom(const ZoomParam& param)
{
    //現在のズーム情報を保持させる
    retainZoomParam_.armLength_    = armLength_;
    retainZoomParam_.socketOffset_ = socketOffset_;
    retainZoomParam_.targetOffset_ = targetOffset_;

    //ズーム後の値を保持させる
    nextZoomParam_.armLength_    = param.armLength_;
    nextZoomParam_.socketOffset_ = param.socketOffset_;
    nextZoomParam_.targetOffset_ = param.targetOffset_;
    nextZoomParam_.time_         = param.time_;

    //タイマーリセット
    zoomTimer_ = 0.0f;

    //ズームフラグを立てる
    isZooming_ = true;
}

void Camera::ZoomReset(const float time)
{
    //現在のズーム情報を保持させる
    retainZoomParam_.armLength_    = armLength_;
    retainZoomParam_.socketOffset_ = socketOffset_;
    retainZoomParam_.targetOffset_ = targetOffset_;

    //ズーム後の値を保持させる
    nextZoomParam_.armLength_    = baseArmLength_;
    nextZoomParam_.socketOffset_ = baseSocketOffset_;
    nextZoomParam_.targetOffset_ = baseTargetOffset_;
    nextZoomParam_.time_ = time;

    //タイマーリセット
    zoomTimer_ = 0.0f;

    //ズームフラグを立てる
    isZooming_ = true;
}

void Camera::ZoomUpdate()
{
    if (!isZooming_)return;

    zoomTimer_ += Time::deltaTime_;

    //補完しきっているか
    if (zoomTimer_ > nextZoomParam_.time_)
    {
        armLength_ = nextZoomParam_.armLength_;

        socketOffset_.x = nextZoomParam_.socketOffset_.x;
        socketOffset_.y = nextZoomParam_.socketOffset_.y;
        socketOffset_.z = nextZoomParam_.socketOffset_.z;

        targetOffset_.x = nextZoomParam_.targetOffset_.x;
        targetOffset_.y = nextZoomParam_.targetOffset_.y;
        targetOffset_.z = nextZoomParam_.targetOffset_.z;

        //ズーム終了
        isZooming_ = false;

        return;
    }

    //補完率を計算
    float compValue = zoomTimer_ / nextZoomParam_.time_;

    //イージングで補完する
    armLength_ = Easing::InOutBack(compValue, 1.0f, 0.5f, retainZoomParam_.armLength_, nextZoomParam_.armLength_);

    socketOffset_.x = Easing::InOutBack(compValue, 1.0f, 0.5f, retainZoomParam_.socketOffset_.x, nextZoomParam_.socketOffset_.x);
    socketOffset_.y = Easing::InOutBack(compValue, 1.0f, 0.5f, retainZoomParam_.socketOffset_.y, nextZoomParam_.socketOffset_.y);
    socketOffset_.z = Easing::InOutBack(compValue, 1.0f, 0.5f, retainZoomParam_.socketOffset_.z, nextZoomParam_.socketOffset_.z);

    targetOffset_.x = Easing::InOutBack(compValue, 1.0f, 0.5f, retainZoomParam_.targetOffset_.x, nextZoomParam_.targetOffset_.x);
    targetOffset_.y = Easing::InOutBack(compValue, 1.0f, 0.5f, retainZoomParam_.targetOffset_.y, nextZoomParam_.targetOffset_.y);
    targetOffset_.z = Easing::InOutBack(compValue, 1.0f, 0.5f, retainZoomParam_.targetOffset_.z, nextZoomParam_.targetOffset_.z);
}

void Camera::DebugCameraController()
{
    if (!enableDebugController_)return;

    //マウス、キーボードによるカメラ操作
    {
        auto rot = transform_->GetRotation();
        //マウス操作

        static POINT startMousePos = {};

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
                    rot.y -= mouseVec.x * Time::deltaTime_ * mouseSensitivity_.x;
                    rot.x -= mouseVec.y * Time::deltaTime_ * mouseSensitivity_.y;
                }
            }
            else
            {
                POINT pos{};
                GetCursorPos(&pos);
                startMousePos = { pos.x,pos.y };
                inputStart = true;
                ShowCursor(false);

            }
            SetCursorPos(960, 540);
        }
        else
        {
            inputStart = false;
        }

        if (Mouse::GetButtonUp(Mouse::BTN_RIGHT))
        {
            SetCursorPos(startMousePos.x,startMousePos.y);
            ShowCursor(true);
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

        const float CAMERA_MAX_ROT_X = 150.0f;
        const float CAMERA_MIN_ROT_X = -150.0f;
        rot.x = std::clamp(rot.x, CAMERA_MIN_ROT_X, CAMERA_MAX_ROT_X);

        transform_->SetRotation(rot);
    }

    //移動処理
    {
        //マウスホイールでの移動
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
        //入力値がない場合処理しない
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

    //const auto& offset = transform_->GetRight() * targetOffset_.x + transform_->GetUp() * targetOffset_.y + transform_->GetForward() * targetOffset_.z;
    //const auto& target = viewTarget_->GetPosition() + offset;
    //回転は後から計算するので無視する
    const auto& target = viewTarget_->GetPosition();

    //カメラからビューターゲットへのベクトル
    const auto& vec = target - cameraPos;

    //移動ベクトル計算
    Vector3 moveVec = Vector3::Lerp(Vector3(0,0,0), vec, 1.0f / cameraLagSpeed_) * Time::deltaTime_ * 100.0f;// * 100.0f単純に値が小さすぎるから増やしてるだけ
    if (moveVec.LengthSquared() > vec.LengthSquared())
    {
        /*Vector3 vecNormal;
        moveVec.Normalize(vecNormal);
        moveVec = vecNormal * vec.Length();*/

        cameraPos = target;
    }
    else
    {
        cameraPos = cameraPos + moveVec;
    }

    transform_->SetPosition(cameraPos);
}

void Camera::CameraShake(CameraShakeParameters shakeParam)
{
    //シード値を更新
    shakePerlinNoise_.SetSeed(rand() % 100);

    //パラメータを更新
    shakeParam_ = shakeParam;

    //タイマーリセット
    shakeTimer_ = 0;
    shakePosFreqTimer_ = { 0,0,0 };
    shakeRotFreqTimer_ = { 0,0,0 };

    activeCameraShake_ = true;
}

void Camera::CameraShakeUpdate()
{
    if (!activeCameraShake_)return;

    //振動する方向を設定　円を描くようになっている;
    const Vector3 freqVec = { sinf(shakePosFreqTimer_.x),cosf(shakePosFreqTimer_.y),sinf(shakePosFreqTimer_.z) };

    //右ベクトル、上ベクトルから振動する方向ベクトルを算出
    const Vector3 forward   = transform_->GetForward();
    const Vector3 right     = transform_->GetRight();
    const Vector3 up        = transform_->GetUp();


    Vector3 shakeVec = Vector3::Normalize(right * freqVec.x + up * freqVec.y + forward * freqVec.z);

    //位置振動の乗数から振動の強さを表す値を作成
    float shakePower = 1;

    //フェードイン
    if (shakeParam_.timing_.blendInTime_ > 0 &&
        shakeTimer_ < shakeParam_.timing_.blendInTime_)
    {
        //フェードイン中の時間を正規化
        const float t = shakeTimer_ / shakeParam_.timing_.blendInTime_;
        shakePower = std::lerp(0.0f, 1.0f, t);
    }
    //フェードアウト
    else if (shakeParam_.timing_.blendOutTime_ > 0 &&
        shakeTimer_ > shakeParam_.timing_.duration_ - shakeParam_.timing_.blendOutTime_)
    {
        //フェードアウト中の時間を正規化
        const auto& tp = shakeParam_.timing_;
        const float t = (shakeTimer_ - (tp.duration_ - tp.blendOutTime_)) / tp.blendOutTime_;
        shakePower = std::lerp(1.0f, 0.0f, t);
    }

    //振動
    shakeVec = shakeVec * shakePower;

    shakePosition_ = {
        shakeVec.x * (shakePerlinNoise_.OctaveNoise(3,shakePosFreqTimer_.x) - 0.5f) * shakeParam_.position_.amplitude_.x * shakeParam_.position_.amplitudeMultiplier_,
        shakeVec.y * (shakePerlinNoise_.OctaveNoise(3,shakePosFreqTimer_.y) - 0.5f) * shakeParam_.position_.amplitude_.y * shakeParam_.position_.amplitudeMultiplier_,
        shakeVec.z * (shakePerlinNoise_.OctaveNoise(3,shakePosFreqTimer_.z) - 0.5f) * shakeParam_.position_.amplitude_.z * shakeParam_.position_.amplitudeMultiplier_
    };

    shakeRotation_ = {
        shakeVec.x * (shakePerlinNoise_.OctaveNoise(3,shakeRotFreqTimer_.x) - 0.5f) * shakeParam_.rotation_.amplitude_.x * shakeParam_.rotation_.amplitudeMultiplier_,
        shakeVec.y * (shakePerlinNoise_.OctaveNoise(3,shakeRotFreqTimer_.y) - 0.5f) * shakeParam_.rotation_.amplitude_.y * shakeParam_.rotation_.amplitudeMultiplier_,
        shakeVec.z * (shakePerlinNoise_.OctaveNoise(3,shakeRotFreqTimer_.z) - 0.5f) * shakeParam_.rotation_.amplitude_.z * shakeParam_.rotation_.amplitudeMultiplier_
    };


    //タイマー更新
    shakeTimer_ += Time::deltaTime_;
    shakePosFreqTimer_ += (shakeParam_.position_.frequency_ * shakeParam_.position_.frequencyMultiplier_) * Time::deltaTime_;
    shakeRotFreqTimer_ += (shakeParam_.rotation_.frequency_ * shakeParam_.rotation_.frequencyMultiplier_) * Time::deltaTime_;

    //振動終了
    if (shakeTimer_ > shakeParam_.timing_.duration_)
    {
        activeCameraShake_ = false;

        shakePosition_ = { 0,0,0 };
        shakeRotation_ = { 0,0,0 };
    }
}

Camera::CameraShakeParameters Camera::CameraShakeParameters::operator=(const CameraShakeParameters& param)
{
    position_.amplitude_ = param.position_.amplitude_;
    position_.amplitudeMultiplier_ = param.position_.amplitudeMultiplier_;
    position_.frequency_ = param.position_.frequency_;
    position_.frequencyMultiplier_ = param.position_.frequencyMultiplier_;

    rotation_.amplitude_ = param.rotation_.amplitude_;
    rotation_.amplitudeMultiplier_ = param.rotation_.amplitudeMultiplier_;
    rotation_.frequency_ = param.rotation_.frequency_;
    rotation_.frequencyMultiplier_ = param.rotation_.frequencyMultiplier_;

    timing_.duration_ = param.timing_.duration_;
    timing_.blendInTime_ = param.timing_.blendInTime_;
    timing_.blendOutTime_ = param.timing_.blendOutTime_;
    return *this;
}
