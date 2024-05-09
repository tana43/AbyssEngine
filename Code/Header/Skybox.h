#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <memory>

namespace AbyssEngine
{
    class FullscreenQuad;

    //スカイボックス
    class Skybox
    {
    public:
        Skybox();
        ~Skybox() = default;

        void Render(FullscreenQuad* bitBlockTransfer);

    private:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
    };
}

