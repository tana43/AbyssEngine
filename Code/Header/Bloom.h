#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>

#include "FrameBuffer.h"
#include "fullscreenQuad.h"

namespace AbyssEngine
{
	class FrameBuffer;
	class FullscreenQuad;

	class Bloom
	{
	public:
		Bloom(uint32_t width, uint32_t height_);
		~Bloom() = default;
		Bloom(const Bloom&) = delete;
		Bloom& operator =(const Bloom&) = delete;
		Bloom(Bloom&&) noexcept = delete;
		Bloom& operator =(Bloom&&) noexcept = delete;

		void Make(ID3D11ShaderResourceView* colorMap);
		ID3D11ShaderResourceView* GetShaderResourceView() const
		{
			return glowExtraction_->GetColorMap().Get();
		}

		void DrawImGui();

	public:
		float bloomExtractionThreshold_ = 0.55f;
		float bloomIntensity_ = 0.01f;

	private:
		std::unique_ptr<FullscreenQuad> bitBlockTransfer_;
		std::unique_ptr<FrameBuffer> glowExtraction_;

		static const size_t DOWNSAMPLED_COUNT = 6;
		std::unique_ptr<FrameBuffer> gaussianBlur_[DOWNSAMPLED_COUNT][2];

		Microsoft::WRL::ComPtr<ID3D11PixelShader> glowExtractionPs_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurDownsamplingPs_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurHorizontalPs_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurVerticalPs_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurUpsamplingPs_;

		struct BloomConstants
		{
			float bloomExtractionThreshold_;
			float bloomIntensity_;
			float something_[2];
		};
		Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;
	};
}