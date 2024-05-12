#pragma once

#pragma once

#include <map>
#include <string>
#include <limits>

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#include "tinygltf-release/tiny_gltf.h"

#include "GeometricSubstance.h"

namespace AbyssEngine
{
	class GltfStaticMesh : public GeometricSubstance
	{
	public:
		GltfStaticMesh(const std::string& filename);

		std::vector<Primitive> batchPrimitives_;

	private:
		void ExtractMeshes(const tinygltf::Model& transmissionModel);

	public:
		int Draw(DrawPass pass, const DirectX::XMFLOAT4X4& transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, std::function<void(const Material&, PipelineState&)> drawcallback = [](const Material&, PipelineState&) {});
		int CastShadow(const DirectX::XMFLOAT4X4& transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });

		int _Draw(DrawPass pass, const DirectX::XMFLOAT4X4& transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> staticMeshVs_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> staticMeshPs_;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> csmOpaqueStaticMeshVs_;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> csmOpaqueGs_;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> csmOpaquInputLayout_;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> csmTransparentStaticMeshVs_;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> csmTransparentGs_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> csmTransparentPs_;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> csmTransparentInputLayout_;
	};
}



