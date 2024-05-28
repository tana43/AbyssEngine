#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>

namespace AbyssEngine
{
    class FrameBuffer
    {
    public:
        FrameBuffer(uint32_t width, uint32_t height_, bool hasDepthstencil = true,bool generateMips = false);
        virtual ~FrameBuffer() = default;

        void Clear(float r = 0, float g = 0, float b = 0, float a = 1, float depth = 1, uint8_t stencil = 0);
        void Activate();
        void Deactivate();

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetColorMap() { return shaderResourceViews_[0]; }
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDepthMap() { return shaderResourceViews_[1]; }

        void GenerateMips();//現在のフレームバッファーのミップマップを生成する
        //void Resolve(FrameBuffer* destination);
        void CopyFrom(const FrameBuffer* source);

    protected:
        Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer_;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer_;

        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView_;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews_[2];
        D3D11_VIEWPORT viewport_;

    private:
        UINT viewportCount_ = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
        D3D11_VIEWPORT cachedViewports_[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView_;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView_;
    };
}
