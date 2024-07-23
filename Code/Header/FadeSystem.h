#pragma once
#include <vector>

namespace AbyssEngine
{
    //���l�̃t�F�[�h�C���A�A�E�g���ł���e���v���[�g�N���X
    class FadeSystem
    {
    public:
        //�t�F�[�h�����������l��o�^
        FadeSystem(float* data);
        FadeSystem(float* data, size_t arraySize);
        ~FadeSystem() {}

        //�t�F�[�h���� �J�ڂ��������Ă����true
        //���t���[���Ăяo������
        bool Fade(const float& time, const float& nextParam);
        bool Fade(const float& time, const std::vector<float>& nextParams);

    private:
        std::vector<float*> params_;

        //�t�F�[�h�J�n���̒l���i�[����
        std::vector<float> originParams_;

        //�J�ڒ���
        bool transitioning_ = false;

        float timer_;
        float time_;
    };
}

