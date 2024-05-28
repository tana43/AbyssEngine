#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

namespace AbyssEngine
{
	class DebugRenderer
	{
	public:
		DebugRenderer(ID3D11Device* device);
		~DebugRenderer() {}

		static DebugRenderer& Get();

	public:
		// •`‰æÀs
		void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

		// ‹…•`‰æ
		void DrawSphere(const DirectX::XMFLOAT3& center_, float radius_, const DirectX::XMFLOAT4& color_);

		// ‰~’Œ•`‰æ
		void DrawCylinder(const DirectX::XMFLOAT3& position_, float radius_, float height_, const DirectX::XMFLOAT4& color_);

	private:
		// ‹…ƒƒbƒVƒ…ì¬
		void CreateSphereMesh(ID3D11Device* device, float radius_, int slices, int stacks);

		// ‰~’ŒƒƒbƒVƒ…ì¬
		void CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height_, int slices, int stacks);

	private:
		struct CbMesh
		{
			DirectX::XMFLOAT4X4	wvp_;
			DirectX::XMFLOAT4	color_;
		};

		struct Sphere
		{
			DirectX::XMFLOAT4	color_;
			DirectX::XMFLOAT3	center_;
			float				radius_;
		};

		struct Cylinder
		{
			DirectX::XMFLOAT4	color_;
			DirectX::XMFLOAT3	position_;
			float				radius_;
			float				height_;
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer>			sphereVertexBuffer_;
		Microsoft::WRL::ComPtr<ID3D11Buffer>			cylinderVertexBuffer_;
		Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer_;

		Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader_;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout_;

		Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState_;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState_;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState_;

		std::vector<Sphere>		spheres_;
		std::vector<Cylinder>	cylinders_;

		UINT	sphereVertexCount_ = 0;
		UINT	cylinderVertexCount_ = 0;
	};
}
