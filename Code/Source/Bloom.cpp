#include "Bloom.h"
#include "Shader.h"
#include "FrameBuffer.h"
#include "FullscreenQuad.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;


Bloom::Bloom(uint32_t width, uint32_t height)
{
	bitBlockTransfer_ = std::make_unique<FullscreenQuad>();

	glowExtraction_ = std::make_unique<FrameBuffer>(width, height, false);
	for (size_t downsampledIndex = 0; downsampledIndex < DOWNSAMPLED_COUNT; ++downsampledIndex)
	{
		gaussianBlur_[downsampledIndex][0] = std::make_unique<FrameBuffer>(width >> downsampledIndex, height >> downsampledIndex, false);
		gaussianBlur_[downsampledIndex][1] = std::make_unique<FrameBuffer>(width >> downsampledIndex, height >> downsampledIndex, false);
	}
	glowExtractionPs_			= Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/GlowExtractionPS.cso");
	gaussianBlurDownsamplingPs_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/GaussianBlurDownsamplingPS.cso");
	gaussianBlurHorizontalPs_	= Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/GaussianBlurHorizontalPS.cso");
	gaussianBlurVerticalPs_		= Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/GaussianBlurVerticalPS.cso");
	gaussianBlurUpsamplingPs_	= Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/GaussianBlurUpsamplingPS.cso");

	D3D11_BUFFER_DESC bd{};
	bd.ByteWidth = sizeof(BloomConstants);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	HRESULT hr = DXSystem::GetDevice()->CreateBuffer(&bd, nullptr, constantBuffer_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}

void Bloom::Make(ID3D11ShaderResourceView* colorMap)
{
	ID3D11DeviceContext* deviceContext = DXSystem::GetDeviceContext();

	// Store current states
	ID3D11ShaderResourceView* nullShaderResourceView{};
	ID3D11ShaderResourceView* cachedShaderResourceViews_[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	deviceContext->PSGetShaderResources(0, DOWNSAMPLED_COUNT, cachedShaderResourceViews_);

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  cachedDepthStencilSstate;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>  cachedRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState>  cachedBlendState;
	FLOAT blendFactor[4];
	UINT sampleMask;
	deviceContext->OMGetDepthStencilState(cachedDepthStencilSstate.GetAddressOf(), 0);
	deviceContext->RSGetState(cachedRasterizerState.GetAddressOf());
	deviceContext->OMGetBlendState(cachedBlendState.GetAddressOf(), blendFactor, &sampleMask);

	Microsoft::WRL::ComPtr<ID3D11Buffer>  cachedConstantBuffer;
	deviceContext->PSGetConstantBuffers(8, 1, cachedConstantBuffer.GetAddressOf());

	// Bind states
	DXSystem::SetDepthStencilState(DS_State::None_No_Write);
	DXSystem::SetRasterizerState(RS_State::Cull_Back);
	DXSystem::SetBlendState(BS_State::Off);

	BloomConstants data_{};
	data_.bloomExtractionThreshold_ = bloomExtractionThreshold_;
	data_.bloomIntensity_ = bloomIntensity_;
	deviceContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, &data_, 0, 0);
	deviceContext->PSSetConstantBuffers(8, 1, constantBuffer_.GetAddressOf());

	// Extracting bright color
	glowExtraction_->Clear(0, 0, 0, 1);
	glowExtraction_->Activate();
	bitBlockTransfer_->Blit(&colorMap, 0, 1, glowExtractionPs_.Get());
	glowExtraction_->Deactivate();
	deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

	// Gaussian blur
	// Efficient Gaussian blur with linear sampling
	// http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
	// Downsampling
	gaussianBlur_[0][0]->Clear(0, 0, 0, 1);
	gaussianBlur_[0][0]->Activate();
	bitBlockTransfer_->Blit(glowExtraction_->GetColorMap().GetAddressOf(), 0, 1, gaussianBlurDownsamplingPs_.Get());
	gaussianBlur_[0][0]->Deactivate();
	deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

	// Ping-pong gaussian blur
	gaussianBlur_[0][1]->Clear(0, 0, 0, 1);
	gaussianBlur_[0][1]->Activate();
	bitBlockTransfer_->Blit(gaussianBlur_[0][0]->GetColorMap().GetAddressOf(), 0, 1, gaussianBlurHorizontalPs_.Get());
	gaussianBlur_[0][1]->Deactivate();
	deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

	gaussianBlur_[0][0]->Clear(0, 0, 0, 1);
	gaussianBlur_[0][0]->Activate();
	bitBlockTransfer_->Blit(gaussianBlur_[0][1]->GetColorMap().GetAddressOf(), 0, 1, gaussianBlurVerticalPs_.Get());
	gaussianBlur_[0][0]->Deactivate();
	deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

	for (size_t downsampled_index = 1; downsampled_index < DOWNSAMPLED_COUNT; ++downsampled_index)
	{
		// Downsampling
		gaussianBlur_[downsampled_index][0]->Clear( 0, 0, 0, 1);
		gaussianBlur_[downsampled_index][0]->Activate();
		bitBlockTransfer_->Blit(gaussianBlur_[downsampled_index - 1][0]->GetColorMap().GetAddressOf(), 0, 1, gaussianBlurDownsamplingPs_.Get());
		gaussianBlur_[downsampled_index][0]->Deactivate();
		deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

		// Ping-pong gaussian blur
		gaussianBlur_[downsampled_index][1]->Clear( 0, 0, 0, 1);
		gaussianBlur_[downsampled_index][1]->Activate();
		bitBlockTransfer_->Blit(gaussianBlur_[downsampled_index][0]->GetColorMap().GetAddressOf(), 0, 1, gaussianBlurHorizontalPs_.Get());
		gaussianBlur_[downsampled_index][1]->Deactivate();
		deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

		gaussianBlur_[downsampled_index][0]->Clear( 0, 0, 0, 1);
		gaussianBlur_[downsampled_index][0]->Activate();
		bitBlockTransfer_->Blit(gaussianBlur_[downsampled_index][1]->GetColorMap().GetAddressOf(), 0, 1, gaussianBlurVerticalPs_.Get());
		gaussianBlur_[downsampled_index][0]->Deactivate();
		deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);
	}

	// Downsampling
	glowExtraction_->Clear( 0, 0, 0, 1);
	glowExtraction_->Activate();
	std::vector<ID3D11ShaderResourceView*> shaderResourceViews_;
	for (size_t downsampledIndex = 0; downsampledIndex < DOWNSAMPLED_COUNT; ++downsampledIndex)
	{
		shaderResourceViews_.push_back(gaussianBlur_[downsampledIndex][0]->GetColorMap().Get());
	}
	bitBlockTransfer_->Blit(shaderResourceViews_.data(), 0, DOWNSAMPLED_COUNT, gaussianBlurUpsamplingPs_.Get());
	glowExtraction_->Deactivate();
	deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

	// Restore states
	deviceContext->PSSetConstantBuffers(8, 1, cachedConstantBuffer.GetAddressOf());

	deviceContext->OMSetDepthStencilState(cachedDepthStencilSstate.Get(), 0);
	deviceContext->RSSetState(cachedRasterizerState.Get());
	deviceContext->OMSetBlendState(cachedBlendState.Get(), blendFactor, sampleMask);

	deviceContext->PSSetShaderResources(0, DOWNSAMPLED_COUNT, cachedShaderResourceViews_);
	for (ID3D11ShaderResourceView* cachedShaderResourceView : cachedShaderResourceViews_)
	{
		if (cachedShaderResourceView) cachedShaderResourceView->Release();
	}
}

void Bloom::DrawImGui()
{
	ImGui::DragFloat("Intensity", &bloomIntensity_, 0.01f, 0.01f);
	ImGui::DragFloat("ExtThreshold", &bloomExtractionThreshold_, 0.01f, 0.01f);
}

