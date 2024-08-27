#include "Input.h"
#include "Engine.h"

using namespace AbyssEngine;
using namespace std;

Input::Input()
{
    keyboard_           = make_unique<Keyboard>();

    mouse_              = make_unique<Mouse>();
}

void Input::Update()
{
    keyboard_->Update();
    mouse_->Update();
    gamePad_.Update();

    //最後に入力されたデバイスの検出
    {
        auto currPtr = reinterpret_cast<const uint32_t*>(&keyboard_->GetKeyState());
        for (size_t j = 0; j < (256 / 32); ++j)
        {
            if (*currPtr)
            {
                useDevice_ = KEYBOARD;
                break;
            }
            ++currPtr;
        }

        if (gamePad_.GetButton())
        {
            useDevice_ = GAMEPAD;
        }
    }
}

GamePad& Input::GetGamePad()
{
    return Engine::inputManager_->gamePad_;
}

const Vector2 Input::GameSupport::GetMoveVector()
{
    //キーボード、コントローラー両方に対応
    Vector2 input;
    const auto& i = Engine::inputManager_;
    if(i->keyboard_->GetKeyState().W)input.y = 1.0f;
    if(i->keyboard_->GetKeyState().A)input.x = -1.0f;
    if(i->keyboard_->GetKeyState().S)input.y = -1.0f;
    if(i->keyboard_->GetKeyState().D)input.x = 1.0f;

    input.x += i->gamePad_.GetAxisLX();
    input.y += i->gamePad_.GetAxisLY();

    //もし入力ベクトルのLengthが1を超えるている場合は正規化しておく
    if (sqrtf(input.x * input.x + input.y * input.y) > 1)
    {
        input.Normalize();
    }

    return input;
}

const bool Input::GameSupport::GetDashButton()
{
    bool input = false;
    auto& i = Engine::inputManager_;
    if (i->keyboard_->GetKeyState().LeftShift ||
        (i->gamePad_.GetButton() & GamePad::BTN_RIGHT_SHOULDER))
    {
        input = true;
    }
    
    return input;
}

const bool Input::GameSupport::GetDodgeButton()
{
    bool input = false;
    auto& i = Engine::inputManager_;
    if (i->keyboard_->GetKeyDown(DirectX::Keyboard::LeftShift) ||
        (i->gamePad_.GetButtonDown() & GamePad::BTN_X))
    {
        input = true;
    }

    return input;
}

const Vector2 Input::GameSupport::GetCameraRollVector()
{
    //キーボード、コントローラー両方に対応
    Vector2 input;
    const auto& i = Engine::inputManager_;
    if (i->keyboard_->GetKeyState().I)input.y = 1.0f;
    if (i->keyboard_->GetKeyState().J)input.x = -1.0f;
    if (i->keyboard_->GetKeyState().K)input.y = -1.0f;
    if (i->keyboard_->GetKeyState().L)input.x = 1.0f;

    input.x += i->gamePad_.GetAxisRX();
    input.y += i->gamePad_.GetAxisRY();

    //もし入力ベクトルのLengthが1を超えるている場合は正規化しておく
    if (sqrtf(input.x * input.x + input.y * input.y) > 1)
    {
        input.Normalize();
    }

    return input;
}

const bool Input::GameSupport::GetJumpButton()
{
    auto& i = Engine::inputManager_;
    if (i->keyboard_->GetKeyDown(DirectX::Keyboard::Space) ||
        i->gamePad_.GetButtonDown() & GamePad::BTN_A)
    {
        return true;
    }

    return false;
}

const bool Input::GameSupport::GetClimdButton()
{
    auto& i = Engine::inputManager_;
    if (i->keyboard_->GetKeyState().Space ||
        i->gamePad_.GetButton() & GamePad::BTN_A)
    {
        return true;
    }

    return false;
}

const bool AbyssEngine::Input::GameSupport::GetAimButton()
{
    auto& i = Engine::inputManager_;
    if (i->mouse_->GetButtonState().rightButton ||
        i->gamePad_.GetButton() & GamePad::BTN_LEFT_THUMB)
    {
        return true;
    }

    return false;
}

const bool AbyssEngine::Input::GameSupport::GetShotButton()
{
    auto& i = Engine::inputManager_;
    if (i->mouse_->GetButtonState().leftButton ||
        i->gamePad_.GetButton() & GamePad::BTN_RIGHT_THUMB)
    {
        return true;
    }

    return false;
}

const bool AbyssEngine::Input::GameSupport::GetBoardingButton()
{
    auto& i = Engine::inputManager_;
    if (i->keyboard_->GetKeyDown(DirectX::Keyboard::Space) ||
        i->gamePad_.GetButtonDown() & GamePad::BTN_A)
    {
        return true;
    }

    return false;
}
