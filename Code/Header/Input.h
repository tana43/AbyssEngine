#pragma once

#include "Mouse.h"
#include "Keyboard.h"
#include "GamePad.h"

using Device = unsigned int;

namespace AbyssEngine
{
    //���̃N���X����e����͂̃A�b�v�f�[�g������
    //���͔�����e�A�N�V�������ƂɌ��߂����Ƃ��������Ŋ֐���`����
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

