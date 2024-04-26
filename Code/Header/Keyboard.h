#pragma once
#include <../External/DirectXTK-main/Inc/Keyboard.h>

using UseKey = unsigned int;

namespace AbyssEngine
{
    //DirectXTKのKeyboardを使う
    class Keyboard
    {
    public:
        static Keyboard* instance_;

    public:
        Keyboard();
        ~Keyboard() {}

        void Update();

        //それぞれのキーの入力状態を持った構造体を返す(例:if(GetKeyState().Space))
        static DirectX::Keyboard::State& GetKeyState();

        //キーを押した瞬間かどうかを返す
        static const bool GetKeyDown(const DirectX::Keyboard::Keys key);

        //キーを離した瞬間かどうかを返す
        static const bool GetKeyUp(const DirectX::Keyboard::Keys key);

    private:
        //DirectX::Keyboardクラスを生成するためのポインタ
        std::unique_ptr<DirectX::Keyboard> keyboard_;

        //ボタンを押した瞬間、離した瞬間の処理をしてくれている
        DirectX::Keyboard::KeyboardStateTracker tracker_;

        //キーボードの入力状態を取ってくる
        DirectX::Keyboard::State state_;
    };
}
