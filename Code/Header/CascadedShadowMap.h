#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include <functional>
#include <memory>

#include "ConstantBuffer.h"

// https://learnopengl.com/Guest-Articles/2021/CSM
// https://learn.microsoft.com/en-us/windows/win32/dxtecharts/cascaded-shadow-maps
// https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-10-parallel-split-shadow-maps-programmable-gpus
namespace AbyssEngine
{
	class CascadedShadowMap
	{

	public:
		CascadedShadowMap(UINT width, UINT height, UINT cascadeCount = 4);
		virtual ~CascadedShadowMap() = default;
		CascadedShadowMap(const CascadedShadowMap&) = delete;
		CascadedShadowMap& operator =(const CascadedShadowMap&) = delete;
		CascadedShadowMap(CascadedShadowMap&&) noexcept = delete;
		CascadedShadowMap& operator =(CascadedShadowMap&&) noexcept = delete;

		void Make(
			const DirectX::XMFLOAT4X4& cameraView,
			const DirectX::XMFLOAT4X4& cameraProjection,
			const DirectX::XMFLOAT4& lightDirection,
			float criticalDepthValue, // If this value is 0, the camera's far panel distance is used.
			std::function<void()> drawcallback);

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer_;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView_;
		D3D11_VIEWPORT viewport_;

		std::vector<DirectX::XMFLOAT4X4> viewProjection_;
		std::vector<float> distances_;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;

		//定数バッファ
		//カスケードシャドウは複数のマップを使うので、持たせる要素数も複数になる　
		struct Constants
		{
			DirectX::XMFLOAT4X4 viewProjectionMatrices_[4];
			float cascadePlaneDistances_[4];
		};
		std::unique_ptr<ConstantBuffer<Constants>> constants_;


	public:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& DepthMap()
		{
			return shaderResourceView_;
		}
		void Clear();
		
	public:
		const UINT cascadeCount;
		float splitSchemeWeight = 0.7f; // logarithmic_split_scheme * _split_scheme_weight + uniform_split_scheme * (1 - _split_scheme_weight)

	};
}
