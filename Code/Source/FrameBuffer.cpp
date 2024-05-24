#include "FrameBuffer.h"
#include "DXSystem.h"
#include "Misc.h"

using namespace AbyssEngine;

FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, bool hasDepthstencil, bool generateMips)
{
	ID3D11Device* device = DXSystem::GetDevice();

    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC texture2dDesc_;
    texture2dDesc_.Width = width;
    texture2dDesc_.Height = height;
    texture2dDesc_.MipLevels = generateMips ? 0 : 1;
    texture2dDesc_.ArraySize = 1;
    texture2dDesc_.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texture2dDesc_.SampleDesc.Count = 1;
	texture2dDesc_.SampleDesc.Quality = 0;
	texture2dDesc_.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc_.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc_.CPUAccessFlags = 0;
	texture2dDesc_.MiscFlags = generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
	hr = device->CreateTexture2D(&texture2dDesc_, 0, renderTargetBuffer_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc_{};
	renderTargetViewDesc_.Format = texture2dDesc_.Format;
	renderTargetViewDesc_.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = device->CreateRenderTargetView(renderTargetBuffer_.Get(), &renderTargetViewDesc_, renderTargetView_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = texture2dDesc_.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(renderTargetBuffer_.Get(), &shaderResourceViewDesc, shaderResourceViews_[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	// BLOOM
	if (hasDepthstencil)
	{
		texture2dDesc_.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texture2dDesc_.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		hr = device->CreateTexture2D(&texture2dDesc_, 0, depthStencilBuffer_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = 0;
		hr = device->CreateDepthStencilView(depthStencilBuffer_.Get(), &depthStencilViewDesc, depthStencilView_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		hr = device->CreateShaderResourceView(depthStencilBuffer_.Get(), &shaderResourceViewDesc, shaderResourceViews_[1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	viewport_.Width = static_cast<float>(width);
	viewport_.Height = static_cast<float>(height);
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
	viewport_.TopLeftX = 0.0f;
	viewport_.TopLeftY = 0.0f;
}
void FrameBuffer::Clear(float r, float g, float b, float a, float depth, uint8_t stencil)
{
	float color_[4]{ r, g, b, a };
	DXSystem::GetDeviceContext()->ClearRenderTargetView(renderTargetView_.Get(), color_);
	if (depthStencilView_) // BLOOM
	{
		DXSystem::GetDeviceContext()->ClearDepthStencilView(depthStencilView_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
	}
}
void FrameBuffer::Activate()
{
	ID3D11DeviceContext* deviceContext = DXSystem::GetDeviceContext();
	viewportCount_ = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	deviceContext->RSGetViewports(&viewportCount_, cachedViewports_);
	deviceContext->OMGetRenderTargets(1, cachedRenderTargetView_.ReleaseAndGetAddressOf(), cachedDepthStencilView_.ReleaseAndGetAddressOf());

	deviceContext->RSSetViewports(1, &viewport_);
	deviceContext->OMSetRenderTargets(1, renderTargetView_.GetAddressOf(), depthStencilView_.Get());
}
void FrameBuffer::Deactivate()
{
	ID3D11DeviceContext* deviceContext = DXSystem::GetDeviceContext();
	deviceContext->RSSetViewports(viewportCount_, cachedViewports_);
	deviceContext->OMSetRenderTargets(1, cachedRenderTargetView_.GetAddressOf(), cachedDepthStencilView_.Get());
}

void FrameBuffer::GenerateMips()
{
	DXSystem::GetDeviceContext()->GenerateMips(shaderResourceViews_[0].Get());
}

void AbyssEngine::FrameBuffer::CopyFrom(const FrameBuffer* source)
{
	if (renderTargetBuffer_ && source->renderTargetBuffer_)
	{
		DXSystem::GetDeviceContext()->CopySubresourceRegion(renderTargetBuffer_.Get(), 0, 0, 0, 0, source->renderTargetBuffer_.Get(), 0, NULL);
	}
	if (depthStencilBuffer_ && source->depthStencilBuffer_)
	{
		DXSystem::GetDeviceContext()->CopySubresourceRegion(depthStencilBuffer_.Get(), 0, 0, 0, 0, source->depthStencilBuffer_.Get(), 0, NULL);
	}
}

//void FrameBuffer::Resolve(FrameBuffer* destination)
//{
//}
