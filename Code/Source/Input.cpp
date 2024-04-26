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

