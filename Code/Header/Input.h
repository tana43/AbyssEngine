#pragma once

#include "Mouse.h"
#include "Keyboard.h"
#include "GamePad.h"

using Device = unsigned int;

namespace AbyssEngine
{
    //このクラスから各種入力のアップデートをする
    //入力判定を各アクションごとに決めたいときもここで関数定義する
    class Input
    {
    public:
        Input();
        ~Input() {}

    public:
        static const Device KEYBOARD = 0;
        static const Device GAMEPAD  = 1;

    public:
        void Update();

        static GamePad& GetGamePad();

    private:
        GamePad gamePad_;

        std::unique_ptr<Keyboard> keyboard_;
        std::unique_ptr<Mouse> mouse_;

        Device useDevice_;
    };
}

