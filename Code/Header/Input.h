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
        struct GameSupport
        {
        public:
            static const Vector2 GetMoveVector();//キーボード、コントローラーの入力値からベクトルの取得
            static const bool GetMoveButtonDown();//移動キーのどれかが入力されたか
            static const bool GetDashButton();//ダッシュ操作
            static const bool GetDodgeButton();//回避操作
            static const Vector2 GetCameraRollVector();//カメラの回転操作
            static const bool GetJumpButton();///ジャンプボタン
            static const bool GetRiseButton();///上昇ボタン

            static const bool GetAimButton();//銃を構えるボタン
            static const bool GetShotButton();//銃を撃つボタン

            static const bool GetBoardingButton();//ヴィテス搭乗ボタン-
        };
        
    private:
        GamePad gamePad_;

        std::unique_ptr<Keyboard> keyboard_;
        std::unique_ptr<Mouse> mouse_;

        //最後に入力されたデバイス（キーマウかコントローラー）
        Device useDevice_;
    };
}

