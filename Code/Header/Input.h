#pragma once

#include "Mouse.h"
#include "Keyboard.h"
#include "GamePad.h"

#include "MathHelper.h"

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

    public:
        //�����ŕ��G�ȓ��͒l��A�A�N�V�����ɑΉ�������͂��`���Ă���
        struct GameSupport
        {
        public:
            static const Vector2 GetMoveVector();//�L�[�{�[�h�A�R���g���[���[�̓��͒l����x�N�g���̎擾
            static const bool GetDashButton();//�_�b�V������
            static const Vector2 GetCameraRollVector();//�J�����̉�]����
            static const bool GetJumpButton();///�W�����v�{�^��
            static const bool GetClimdButton();///�㏸�{�^��
        };
        
    private:
        GamePad gamePad_;

        std::unique_ptr<Keyboard> keyboard_;
        std::unique_ptr<Mouse> mouse_;

        //�Ō�ɓ��͂��ꂽ�f�o�C�X�i�L�[�}�E���R���g���[���[�j
        Device useDevice_;
    };
}

