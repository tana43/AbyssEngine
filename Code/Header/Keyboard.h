#pragma once
#include <../External/DirectXTK-main/Inc/Keyboard.h>

using UseKey = unsigned int;

namespace AbyssEngine
{
    //DirectXTK��Keyboard���g��
    class Keyboard
    {
    public:
        static Keyboard* instance_;

    public:
        Keyboard();
        ~Keyboard() {}

        void Update();

        //���ꂼ��̃L�[�̓��͏�Ԃ��������\���̂�Ԃ�(��:if(GetKeyState().Space))
        static DirectX::Keyboard::State& GetKeyState();

        //�L�[���������u�Ԃ��ǂ�����Ԃ�
        static const bool GetKeyDown(const DirectX::Keyboard::Keys key);

        //�L�[�𗣂����u�Ԃ��ǂ�����Ԃ�
        static const bool GetKeyUp(const DirectX::Keyboard::Keys key);

    private:
        //DirectX::Keyboard�N���X�𐶐����邽�߂̃|�C���^
        std::unique_ptr<DirectX::Keyboard> keyboard_;

        //�{�^�����������u�ԁA�������u�Ԃ̏��������Ă���Ă���
        DirectX::Keyboard::KeyboardStateTracker tracker_;

        //�L�[�{�[�h�̓��͏�Ԃ�����Ă���
        DirectX::Keyboard::State state_;
    };
}
