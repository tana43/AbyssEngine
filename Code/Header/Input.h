#pragma once

#include "Mouse.h"
#include "Keyboard.h"
#include "GamePad.h"

#include "MathHelper.h"

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

    public:
        //ここで複雑な入力値や、アクションに対応する入力を定義していく

        //キーボード、コントローラーの入力値からベクトルの取得
        static Vector2 GetMoveVector();
        static const bool GetDashButton();

    private:
        GamePad gamePad_;

        std::unique_ptr<Keyboard> keyboard_;
        std::unique_ptr<Mouse> mouse_;

        //最後に入力されたデバイス（キーマウかコントローラー）
        Device useDevice_;
    };
}

