#pragma once

#include <d3d11.h>
#include <directxmath.h>

#include <vector>
#include <string>

namespace AbyssEngine
{
	class GltfCollisionMesh
	{
	public:
		struct Mesh
		{
			std::string name_;
			struct Subset
			{
				std::string materialName_;
				std::vector<DirectX::XMFLOAT3> positions_;
			};
			std::vector<Subset> subsets;
			DirectX::XMFLOAT3 boundingBox_[2]
			{
				{ +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX },
				{ -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX }
			};
		};
		std::vector<Mesh> meshes_;  

	public:
		GltfCollisionMesh (ID3D11Device* device, const std::string& fileName, bool triangulate = false);

		// The coordinate system of all function arguments is world space.
		bool Raycast(_In_ DirectX::XMFLOAT3 rayPosition, _In_ DirectX::XMFLOAT3 rayDirection, _In_ const DirectX::XMFLOAT4X4& transform, _Out_ DirectX::XMFLOAT3& intersectionPosition, _Out_ DirectX::XMFLOAT3& intersectionNormal,
			_Out_ std::string& intersectionMesh, _Out_ std::string& intersectionMaterial, _In_ float rayLengthLimit = 1.0e+7f, _In_ bool skipIf = false/*Once the first intersection is found, the process is interrupted.*/) const;

		bool Raycast(_In_ DirectX::XMFLOAT3 rayPosition, _In_ DirectX::XMFLOAT3 rayDirection, _In_ const DirectX::XMFLOAT4X4& transform, _Out_ DirectX::XMFLOAT3& intersectionPosition, _Out_ DirectX::XMFLOAT3& intersectionNormal,
			_Out_ std::string& intersectionMesh, _Out_ std::string& intersectionMaterial, _Out_ float& distance, _In_ float rayLengthLimit = 1.0e+7f, _In_ bool skipIf = false/*Once the first intersection is found, the process is interrupted.*/) const;

		//äeí∏ì_ÇçsóÒïœä∑
		void Transform(const DirectX::XMFLOAT4X4& worldTransform);
	};
}