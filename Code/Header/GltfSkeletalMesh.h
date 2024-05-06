#pragma once

#include "GeometricSubstance.h"
#include "ConstantBuffer.h"

namespace AbyssEngine
{
	class GltfSkeletalMesh : public GeometricSubstance
	{
	public:
		GltfSkeletalMesh(const std::string& filename);

	private:
		void ExtractMeshes(const tinygltf::Model& transmissionModel);

	public:
		int Draw(DrawPass pass, const DirectX::XMFLOAT4X4& transform, const DirectX::XMFLOAT4& color = { 1, 1, 1, 1 }, size_t sceneIndex = 0);
		int Draw(DrawPass pass, const DirectX::XMFLOAT4X4& transform, const std::vector<Node>& animatedNodes, const DirectX::XMFLOAT4& color = { 1, 1, 1, 1 }, size_t sceneIndex = 0);
		int CastShadow(const DirectX::XMFLOAT4X4& world, const std::vector<Node>& animatedNodes, size_t sceneIndex = 0);

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> skeletalMeshVs_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> skeletalMeshPs_;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> csmOpaqueSkeletalMeshVs_;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> csmOpaqueGs_;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> csmOpaqueInputLayout_;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> csmTransparentSkeletalMeshVs_;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> csmTransparentGs_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> csmTransparentPs_;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> csm_transparent_input_layout;

		static const size_t Max_Joints = 512;
		struct JointConstants
		{
			DirectX::XMFLOAT4X4 matrices_[Max_Joints];
		};
		std::unique_ptr<ConstantBuffer<JointConstants>> jointConstants_;
		const int Joint_Slot = 1; // b1
	};
}