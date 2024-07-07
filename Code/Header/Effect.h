#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>

namespace AbyssEngine
{
    class Effect
    {
    public:
        Effect(const char* filename);
        ~Effect() {}

        //�Đ�
        Effekseer::Handle Play(DirectX::XMFLOAT3 position = { 0,0,0 }, float scale = 1.0f);

        //��~
        void Stop(Effekseer::Handle handle);

        //���W�ݒ�
        void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

        //�X�P�[���ݒ�
        void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

    private:
        Effekseer::EffectRef effekseerEffect;
    };
}

