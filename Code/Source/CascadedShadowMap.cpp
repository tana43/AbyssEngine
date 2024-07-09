#include "CascadedShadowMap.h"

#include <array>

#include "Misc.h"
#include "DXSystem.h"

#include "Engine.h"
#include "RenderManager.h"

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

CascadedShadowMap::CascadedShadowMap(UINT width, UINT height_, UINT cascade_count) : cascadeCount(cascade_count)
{
	ID3D11Device* device = DXSystem::GetDevice();

	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC texture2d_desc = {};
	texture2d_desc.Width = width;
	texture2d_desc.Height = height_;
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

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
	depthStencilViewDesc.Texture2DArray.ArraySize = static_cast<UINT>(cascadeCount);
	depthStencilViewDesc.Texture2DArray.MipSlice = 0;
	depthStencilViewDesc.Flags = 0;
	hr = device->CreateDepthStencilView(depthStencilBuffer_.Get(), &depthStencilViewDesc, depthStencilView_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT; // DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT : DXGI_FORMAT_R16_UNORM
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	shaderResourceViewDesc.Texture2DArray.ArraySize = static_cast<UINT>(cascadeCount);
	shaderResourceViewDesc.Texture2DArray.MipLevels = 1;
	shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
	shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
	hr = device->CreateShaderResourceView(depthStencilBuffer_.Get(), &shaderResourceViewDesc, shaderResourceView_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	viewport_.Width = static_cast<float>(width);
	viewport_.Height = static_cast<float>(height_);
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
	viewport_.TopLeftX = 0.0f;
	viewport_.TopLeftY = 0.0f;

	constants_ = std::make_unique<ConstantBuffer<Constants>>();

}

void CascadedShadowMap::Clear()
{
	DXSystem::GetDeviceContext()->ClearDepthStencilView(depthStencilView_.Get(), D3D11_CLEAR_DEPTH, 1, 0);
}

void CascadedShadowMap::Make(
	const DirectX::XMFLOAT4X4& cameraView,
	const DirectX::XMFLOAT4X4& cameraProjection,
	const DirectX::XMFLOAT4& lightDirection,
	float criticalDepthValue,
	std::function<void()> drawcallback)
{
	ID3D11DeviceContext* deviceContext = DXSystem::GetDeviceContext();

	D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	UINT viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	deviceContext->RSGetViewports(&viewportCount, cachedViewports);
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;
	deviceContext->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());

	// 透視投影行列
	float m33 = cameraProjection._33;
	float m43 = cameraProjection._43;
	float zn = -m43 / m33;
	float zf = (m33 * zn) / (m33 - 1);
	//zf = criticalDepthValue > 0 ? min(zf, criticalDepthValue) : zf;
	zf = criticalDepthValue;

	// calculates split plane distances in view space
	distances_.resize(static_cast<size_t>(cascadeCount) + 1);
	for (size_t cascadeIndex = 0; cascadeIndex < cascadeCount; ++cascadeIndex)
	{
		float idc = cascadeIndex / static_cast<float>(cascadeCount);
		float logarithmicSplitScheme = zn * pow(zf / zn, idc);
		float uniformSplitScheme = zn + (zf - zn) * idc;
		distances_.at(cascadeIndex) = logarithmicSplitScheme * splitSchemeWeight_ + uniformSplitScheme * (1 - splitSchemeWeight_);
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

		DirectX::XMFLOAT4 center_ = { 0, 0, 0, 1 };
		for (decltype(corners)::const_reference v : corners)
		{
			center_.x += v.x;
			center_.y += v.y;
			center_.z += v.z;
		}
		center_.x /= corners.size();
		center_.y /= corners.size();
		center_.z /= corners.size();

		XMMATRIX V;
		V = XMMatrixLookAtLH(
			XMVectorSet(center_.x - lightDirection.x, center_.y - lightDirection.y, center_.z - lightDirection.z, 1.0f),
			XMVectorSet(center_.x, center_.y, center_.z, 1.0f),
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
		//実際の射影行列を作成する前に、光錐台の近平面と遠平面によってカバーされる空間のサイズを拡大します。
		//これは、近い平面を「引き戻す」、遠い平面を「押し出す」ことで行います。コードでは、これを zMult で除算または乗算することによって実現します。
		//これは、カメラ空間内の錐台の後ろまたは前にあるジオメトリを含めたいためです。 考えてみてください。ジオメトリだけではありません。
		//錐台内にある場合は、錐台内のサーフェスに影を落とすことができます。
		constexpr float zMult = 50.0f;
		if (minZ < 0)
		{
			minZ *= zMult;
		}
		else
		{
			minZ /= zMult;
		}
		if (maxZ < 0)
		{
			maxZ /= zMult;
		}
		else
		{
			maxZ *= zMult;
		}
#endif

		XMMATRIX P = XMMatrixOrthographicOffCenterLH(minX, maxX, minY, maxY, minZ, maxZ);
		XMStoreFloat4x4(&viewProjection_.at(cascadeIndex), V * P);

		//カリング用ボックスの設定
		Engine::renderManager_->SetShadowCullingArea(Vector3(maxX,maxY,maxZ),Vector3(minX,minY,minZ));
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

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> nullRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> nullDepthStencilView;
	deviceContext->ClearDepthStencilView(depthStencilView_.Get(), D3D11_CLEAR_DEPTH, 1, 0);
	deviceContext->OMSetRenderTargets(1, nullRenderTargetView.GetAddressOf(), depthStencilView_.Get());
	deviceContext->RSSetViewports(1, &viewport_);

	drawcallback();

	deviceContext->RSSetViewports(viewportCount, cachedViewports);
	deviceContext->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}
