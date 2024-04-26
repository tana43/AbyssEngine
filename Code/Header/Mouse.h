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

        //���ꂼ��̃}�E�X�̓��͏�Ԃ��������\���̂�Ԃ�(��:if(GetButtonState().Space))
        static DirectX::Mouse::State& GetButtonState();
        
        //�L�[���������u�Ԃ��ǂ�����Ԃ�
        static bool GetButtonDown(const MouseButton button);

        //�L�[�𗣂����u�Ԃ��ǂ�����Ԃ�
        static bool GetButtonUp(const MouseButton button);

        static int GetPosX() { return instance_->state_.x; }
        static int GetPosY() { return instance_->state_.y; }

        //TODO:��őO��]�A���]�̂ǂ��炪���̒l��Ԃ����m�F����
        //�}�E�X�z�C�[���̉�]�l��Ԃ�
        static int GetScrollWheelValue() { return instance_->state_.scrollWheelValue; }

    private:
        //DirectX::Keyboard�N���X�𐶐����邽�߂̃|�C���^
        std::unique_ptr<DirectX::Mouse> mouse_;

        //�{�^�����������u�ԁA�������u�Ԃ̔��菈�������Ă���Ă���
        DirectX::Mouse::ButtonStateTracker tracker_;

        //�}�E�X�{�^���̓��͏�Ԃ�����Ă���
        DirectX::Mouse::State state_;
    };
}

