#pragma once

#include <../External/DirectXTK-main/Inc/Mouse.h>
#include <memory>

using MouseButton = char;

namespace AbyssEngine
{
    class Mouse
    {
    public:
        static Mouse* instance_;

    public:
        Mouse();
        ~Mouse() {}

    public:
        static const MouseButton BTN_LEFT = 0;
        static const MouseButton BTN_MIDDLE = 1;
        static const MouseButton BTN_RIGHT = 2;

        void Update();

        //それぞれのマウスの入力状態を持った構造体を返す(例:if(GetButtonState().Space))
        static DirectX::Mouse::State& GetButtonState();
        
        //キーを押した瞬間かどうかを返す
        static bool GetButtonDown(const MouseButton button);

        //キーを離した瞬間かどうかを返す
        static bool GetButtonUp(const MouseButton button);

        static int GetPosX() { return instance_->state_.x; }
        static int GetPosY() { return instance_->state_.y; }
        
        //マウスホイールの回転値を返す　奥へ回すと正の値を返す
        static int GetScrollWheelValue() { return instance_->mouseWheelValue_; }

    private:
        //DirectX::Keyboardクラスを生成するためのポインタ
        std::unique_ptr<DirectX::Mouse> mouse_;

        //ボタンを押した瞬間、離した瞬間の判定処理をしてくれている
        DirectX::Mouse::ButtonStateTracker tracker_;

        //マウスボタンの入力状態を取ってくる
        DirectX::Mouse::State state_;

        //DirectXTK Mouseのマウスホイール回転値は基準値からどのぐらい回っているかの値を返すので
        //１フレームでどの程度回ったかを計算するための変数を用意している
        int curMouseWheelValue_;//１フレーム前のホイール回転値
        int mouseWheelValue_;//１フレーム間にホイールが回転した値
    };
}

