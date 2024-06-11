#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

namespace AbyssEngine
{
	class LineRenderer
	{
	public:
		LineRenderer(ID3D11Device* device, UINT vertexCount);
		~LineRenderer() {}

		//レンダラーマネージャーが持っているLineRendererを取得
		static LineRenderer& Get();
	public:
		// 描画実行
		void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

		// 頂点追加
		void AddVertex(const DirectX::XMFLOAT3& position_, const DirectX::XMFLOAT4& color_);

		void Clear() { vertices_.clear(); }
	private:
		struct ConstantBuffer
		{
			DirectX::XMFLOAT4X4	wvp_;
		};

		struct Vertex
		{
			DirectX::XMFLOAT3	position_;
			DirectX::XMFLOAT4	color_;
		};


		Microsoft::WRL::ComPtr<ID3D11Buffer>			vertexBuffer_;
		Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer_;

		Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader_;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout_;

		Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState_;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState_;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState_;

		std::vector<Vertex>			vertices_;
		UINT						capacity_ = 0;
	};
}
