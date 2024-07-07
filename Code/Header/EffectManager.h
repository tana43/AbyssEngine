#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

namespace AbyssEngine
{
    class EffectManager
    {
    private:
        EffectManager() {}
        ~EffectManager() {}

    public:
        static EffectManager& Instance()
        {
            static EffectManager instance;
            return instance;
        }

        //������
        void Initialize();

        //�I����
        void Finalize();

        //�X�V����
        void Update(float elapsedTime);

        //�`�揈��
        void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

        //Effekseer�}�l�[�W���[�̎擾
        Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager; }

    private:
        Effekseer::ManagerRef          effekseerManager;
        EffekseerRenderer::RendererRef effekseerRenderer;
    };
}

