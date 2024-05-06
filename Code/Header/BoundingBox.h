#pragma once

#include <directxmath.h>
#include <array>

namespace AbyssEngine
{
	// Fill our scratchpad array with the corners of an axis-aligned bounding box.  
	inline void PopulateCorners(_Out_ std::array<DirectX::XMVECTOR, 8>& corners, _In_ const DirectX::XMVECTOR& minValue, _In_ const DirectX::XMVECTOR& maxValue)
	{
		corners[0] = minValue;
		corners[1] = DirectX::XMVectorSet(DirectX::XMVectorGetX(minValue), DirectX::XMVectorGetY(minValue), DirectX::XMVectorGetZ(maxValue), 1);
		corners[2] = DirectX::XMVectorSet(DirectX::XMVectorGetX(minValue), DirectX::XMVectorGetY(maxValue), DirectX::XMVectorGetZ(minValue), 1);
		corners[3] = DirectX::XMVectorSet(DirectX::XMVectorGetX(maxValue), DirectX::XMVectorGetY(minValue), DirectX::XMVectorGetZ(minValue), 1);
		corners[4] = DirectX::XMVectorSet(DirectX::XMVectorGetX(minValue), DirectX::XMVectorGetY(maxValue), DirectX::XMVectorGetZ(maxValue), 1);
		corners[5] = DirectX::XMVectorSet(DirectX::XMVectorGetX(maxValue), DirectX::XMVectorGetY(minValue), DirectX::XMVectorGetZ(maxValue), 1);
		corners[6] = DirectX::XMVectorSet(DirectX::XMVectorGetX(maxValue), DirectX::XMVectorGetY(maxValue), DirectX::XMVectorGetZ(minValue), 1);
		corners[7] = maxValue;
	}
	// Compute a new axis-aligned bounding box that will contain whatever the original
	// bounds did, after an affine transformation. (Note this is a lossy operation)
	inline void ComputeTransformedBounds(_Inout_ DirectX::XMVECTOR& minValue, _Inout_ DirectX::XMVECTOR& maxValue, _In_ const DirectX::XMMATRIX& transform)
	{
		std::array<DirectX::XMVECTOR, 8> corners;
		PopulateCorners(corners, minValue, maxValue);

		minValue = +DirectX::g_XMFltMax;
		maxValue = -DirectX::g_XMFltMax;

		// Transform all of the corners, and keep track of the greatest and least values we see on each coordinate axis.
		for (int i = 0; i < 8; i++)
		{
			DirectX::XMVECTOR transformed = XMVector3TransformCoord(corners[i], transform);
			minValue = DirectX::XMVectorMin(minValue, transformed);
			maxValue = DirectX::XMVectorMax(maxValue, transformed);
		}
	}
}