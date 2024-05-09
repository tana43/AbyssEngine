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

class static_mesh : public geometric_substance
{
public:
	static_mesh(ID3D11Device* device, const std::string& filename);

	std::vector<primitive> batch_primitives;

private:
	void extract_meshes(ID3D11Device* device, const tinygltf::Model& transmission_model);

public:
	int draw(ID3D11DeviceContext* immediate_context, draw_pass pass, const DirectX::XMFLOAT4X4& transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, std::function<void(const material&, pipeline_state&)> drawcallback = [](const material&, pipeline_state&) {});
	int cast_shadow(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });

	int _draw(ID3D11DeviceContext* immediate_context, draw_pass pass, const DirectX::XMFLOAT4X4& transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> static_mesh_vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> static_mesh_ps;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> csm_opaque_static_mesh_vs;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> csm_opaque_gs;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> csm_opaque_input_layout;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> csm_transparent_static_mesh_vs;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> csm_transparent_gs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> csm_transparent_ps;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> csm_transparent_input_layout;
};


