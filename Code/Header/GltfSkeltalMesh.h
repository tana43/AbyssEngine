//#pragma once
//
//#include "GeometricSubstance.h"
//
//namespace AbyssEngine
//{
//	class GltfSkeltalMesh : public GeometricSubstance
//	{
//	public:
//		GltfSkeltalMesh(ID3D11Device* device, const std::string& filename);
//
//	private:
//		void ExtractMeshes(ID3D11Device* device, const tinygltf::Model& transmission_model);
//
//	public:
//		int Draw(ID3D11DeviceContext* immediate_context, DrawPass pass, const DirectX::XMFLOAT4X4& transform, const DirectX::XMFLOAT4& color = { 1, 1, 1, 1 }, size_t scene_index = 0);
//		int Draw(ID3D11DeviceContext* immediate_context, DrawPass pass, const DirectX::XMFLOAT4X4& transform, const std::vector<Node>& animated_nodes, const DirectX::XMFLOAT4& color = { 1, 1, 1, 1 }, size_t scene_index = 0);
//		int CastShadow(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<Node>& animated_nodes, size_t scene_index = 0);
//
//	private:
//		Microsoft::WRL::ComPtr<ID3D11VertexShader> skeletal_mesh_vs;
//		Microsoft::WRL::ComPtr<ID3D11PixelShader> skeletal_mesh_ps;
//		Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
//
//		Microsoft::WRL::ComPtr<ID3D11VertexShader> csm_opaque_skeletal_mesh_vs;
//		Microsoft::WRL::ComPtr<ID3D11GeometryShader> csm_opaque_gs;
//		Microsoft::WRL::ComPtr<ID3D11InputLayout> csm_opaque_input_layout;
//
//		Microsoft::WRL::ComPtr<ID3D11VertexShader> csm_transparent_skeletal_mesh_vs;
//		Microsoft::WRL::ComPtr<ID3D11GeometryShader> csm_transparent_gs;
//		Microsoft::WRL::ComPtr<ID3D11PixelShader> csm_transparent_ps;
//		Microsoft::WRL::ComPtr<ID3D11InputLayout> csm_transparent_input_layout;
//
//		static const size_t _max_joints = 512;
//		struct JointConstants
//		{
//			DirectX::XMFLOAT4X4 matrices_[_max_joints];
//		};
//		std::unique_ptr<ConstantBuffer<JointConstants>> jointConstants_;
//		const int Joint_Slot = 1; // b1
//	};
//}