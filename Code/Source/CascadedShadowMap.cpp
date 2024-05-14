#include "CascadedShadowMap.h"

#include <array>

#include "Misc.h"
#include "DXSystem.h"

using namespace DirectX;
using namespace AbyssEngine;

std::array<XMFLOAT4, 8> MakeFrustumCornersWorldSpace(const XMFLOAT4X4& view, const XMFLOAT4X4& projection)
{
	const XMMATRIX VP = XMMatrixInverse(NULL, XMLoadFloat4x4(&view) * XMLoadFloat4x4(&projection));

	std::array<XMFLOAT4, 8> frustumCorners;
	size_t index = 0;
	for (size_t x = 0; x < 2; ++x)
	{
		for (size_t y = 0; y < 2; ++y)
		{
			for (size_t z = 0; z < 2; ++z)
			{
				XMFLOAT4 pt = { 2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f };
				XMStoreFloat4(&pt, XMVector3TransformCoord(XMLoadFloat4(&pt), VP));
				frustumCorners.at(index++) = pt;
			}
		}
	}
	return frustumCorners;
}

CascadedShadowMap::CascadedShadowMap(UINT width, UINT height, UINT cascade_count) : cascadeCount(cascade_count)
{
	ID3D11Device* device = DXSystem::device_.Get();

	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC texture2d_desc = {};
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = cascadeCount;
	texture2d_desc.Format = DXGI_FORMAT_R32_TYPELESS;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2d_desc, 0, depthStencilBuffer_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
	depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	depth_stencil_view_desc.Texture2DArray.FirstArraySlice = 0;
	depth_stencil_view_desc.Texture2DArray.ArraySize = static_cast<UINT>(cascadeCount);
	depth_stencil_view_desc.Texture2DArray.MipSlice = 0;
	depth_stencil_view_desc.Flags = 0;
	hr = device->CreateDepthStencilView(depthStencilBuffer_.Get(), &depth_stencil_view_desc, depthStencilView_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
	shader_resource_view_desc.Format = DXGI_FORMAT_R32_FLOAT; // DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT : DXGI_FORMAT_R16_UNORM
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	shader_resource_view_desc.Texture2DArray.ArraySize = static_cast<UINT>(cascadeCount);
	shader_resource_view_desc.Texture2DArray.MipLevels = 1;
	shader_resource_view_desc.Texture2DArray.FirstArraySlice = 0;
	shader_resource_view_desc.Texture2DArray.MostDetailedMip = 0;
	hr = device->CreateShaderResourceView(depthStencilBuffer_.Get(), &shader_resource_view_desc, shaderResourceView_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	viewport_.Width = static_cast<float>(width);
	viewport_.Height = static_cast<float>(height);
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
	viewport_.TopLeftX = 0.0f;
	viewport_.TopLeftY = 0.0f;

	constants_ = std::make_unique<ConstantBuffer<Constants>>();

}

void CascadedShadowMap::Clear()
{
	DXSystem::deviceContext_->ClearDepthStencilView(depthStencilView_.Get(), D3D11_CLEAR_DEPTH, 1, 0);
}

void CascadedShadowMap::Make(
	const DirectX::XMFLOAT4X4& cameraView,
	const DirectX::XMFLOAT4X4& cameraProjection,
	const DirectX::XMFLOAT4& lightDirection,
	float criticalDepthValue,
	std::function<void()> drawcallback)
{
	ID3D11DeviceContext* deviceContext = DXSystem::deviceContext_.Get();

	D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	UINT viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	deviceContext->RSGetViewports(&viewportCount, cachedViewports);
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;
	deviceContext->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());

	// near/far value from perspective projection matrix
	float m33 = cameraProjection._33;
	float m43 = cameraProjection._43;
	float zn = -m43 / m33;
	float zf = (m33 * zn) / (m33 - 1);
	zf = criticalDepthValue > 0 ? min(zf, criticalDepthValue) : zf;

	// calculates split plane distances in view space
	distances_.resize(static_cast<size_t>(cascadeCount) + 1);
	for (size_t cascadeIndex = 0; cascadeIndex < cascadeCount; ++cascadeIndex)
	{
		float idc = cascadeIndex / static_cast<float>(cascadeCount);
		float logarithmicSplitScheme = zn * pow(zf / zn, idc);
		float uniformSplitScheme = zn + (zf - zn) * idc;
		distances_.at(cascadeIndex) = logarithmicSplitScheme * splitSchemeWeight + uniformSplitScheme * (1 - splitSchemeWeight);
	}
	// make sure border values are accurate
	distances_.at(0) = zn;
	distances_.at(cascadeCount) = zf;

	const bool fitToCascade = true; // fit to scene : fit to cascade
	viewProjection_.resize(cascadeCount);
	for (size_t cascadeIndex = 0; cascadeIndex < cascadeCount; ++cascadeIndex)
	{
		float _zn = fitToCascade ? distances_.at(cascadeIndex) : zn;
		float _zf = distances_.at(cascadeIndex + 1);

		DirectX::XMFLOAT4X4 cascadedProjection = cameraProjection;
		cascadedProjection._33 = _zf / (_zf - _zn);
		cascadedProjection._43 = -_zn * _zf / (_zf - _zn);

		std::array<XMFLOAT4, 8> corners = MakeFrustumCornersWorldSpace(cameraView, cascadedProjection);

		DirectX::XMFLOAT4 center = { 0, 0, 0, 1 };
		for (decltype(corners)::const_reference v : corners)
		{
			center.x += v.x;
			center.y += v.y;
			center.z += v.z;
		}
		center.x /= corners.size();
		center.y /= corners.size();
		center.z /= corners.size();

		XMMATRIX V;
		V = XMMatrixLookAtLH(
			XMVectorSet(center.x - lightDirection.x, center.y - lightDirection.y, center.z - lightDirection.z, 1.0f),
			XMVectorSet(center.x, center.y, center.z, 1.0f),
			XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		float minX = FLT_MAX;
		float maxX = std::numeric_limits<float>::lowest();
		float minY = FLT_MAX;
		float maxY = std::numeric_limits<float>::lowest();
		float minZ = FLT_MAX;
		float maxZ = std::numeric_limits<float>::lowest();
		for (decltype(corners)::const_reference v : corners)
		{
			XMFLOAT4 _v;
			XMStoreFloat4(&_v, XMVector3TransformCoord(XMLoadFloat4(&v), V));
			minX = min(minX, _v.x);
			maxX = max(maxX, _v.x);
			minY = min(minY, _v.y);
			maxY = max(maxY, _v.y);
			minZ = min(minZ, _v.z);
			maxZ = max(maxZ, _v.z);
		}

#if 1
		// Before creating the actual projection matrix we are going to increase the size of the space covered by the nearand far plane of the light frustum.
		// We do this by "pulling back" the near plane, and "pushing away" the far plane.In the code we achieve this by dividing or multiplying by zMult.
		// This is because we want to include geometry which is behind or in front of our frustum in camera space. Think about it : not only geometry which 
		// is in the frustum can cast shadows on a surface in the frustum!
		constexpr float z_mult = 50.0f;
		if (minZ < 0)
		{
			minZ *= z_mult;
		}
		else
		{
			minZ /= z_mult;
		}
		if (maxZ < 0)
		{
			maxZ /= z_mult;
		}
		else
		{
			maxZ *= z_mult;
		}
#endif

		XMMATRIX P = XMMatrixOrthographicOffCenterLH(minX, maxX, minY, maxY, minZ, maxZ);
		XMStoreFloat4x4(&viewProjection_.at(cascadeIndex), V * P);
	}

	constants_->data_.viewProjectionMatrices_[0] = viewProjection_.at(0);
	constants_->data_.viewProjectionMatrices_[1] = viewProjection_.at(1);
	constants_->data_.viewProjectionMatrices_[2] = viewProjection_.at(2);
	constants_->data_.viewProjectionMatrices_[3] = viewProjection_.at(3);

	constants_->data_.cascadePlaneDistances_[0] = distances_.at(1);
	constants_->data_.cascadePlaneDistances_[1] = distances_.at(2);
	constants_->data_.cascadePlaneDistances_[2] = distances_.at(3);
	constants_->data_.cascadePlaneDistances_[3] = distances_.at(4);

	constants_->Activate(12, CBufferUsage::vp);

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> null_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> null_depth_stencil_view;
	deviceContext->ClearDepthStencilView(depthStencilView_.Get(), D3D11_CLEAR_DEPTH, 1, 0);
	deviceContext->OMSetRenderTargets(1, null_render_target_view.GetAddressOf(), depthStencilView_.Get());
	deviceContext->RSSetViewports(1, &viewport_);

	drawcallback();

	deviceContext->RSSetViewports(viewportCount, cachedViewports);
	deviceContext->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}
