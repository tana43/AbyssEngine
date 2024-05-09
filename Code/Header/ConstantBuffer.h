#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <assert.h>

#include "Misc.h"
#include "DXSystem.h"

/*
	// framework.h
	struct constants_t
	{
		float params[4];
	};
	std::unique_ptr <constant_buffer<constants_t>> constants;

	// framework::initialize
	constants = std::make_unique<constant_buffer<constants_t>>(device.Get());

	// framework::update/render
	constants->data.params[0] = 0;
	constants->activate(immediate_context.Get(), 0, true, true);
*/

#define USAGE_DYNAMIC


namespace AbyssEngine
{
	enum class CBufferUsage : byte
	{
		v = 0x01,
		h = 0x02,
		d = 0x04,
		g = 0x08,
		p = 0x10,
		c = 0x20,
		vp = v | p,
		vgp = v | g | p,
		vhdp = v | h | d | p,
		vhdgp = v | h | d | g | p,
		vhdgpc = v | h | d | g | p | c,
	};

	template <class T>
	struct ConstantBuffer
	{
		T data_;
		ConstantBuffer()
		{
			ID3D11Device* device = DXSystem::device_.Get();
#if 0
			assert(sizeof(T) % 16 == 0 && L"constant buffer's need to be 16 byte aligned");
#endif

			HRESULT hr = S_OK;
			D3D11_BUFFER_DESC bd = {};
			bd.ByteWidth = (sizeof(T) + 0x0f) & ~0x0f;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.MiscFlags = 0;
			bd.StructureByteStride = 0;
#ifdef USAGE_DYNAMIC
			//https://docs.microsoft.com/en-us/windows/desktop/direct3d11/how-to--use-dynamic-resources
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			hr = device->CreateBuffer(&bd, 0, bufferObject_.GetAddressOf());
			assert(SUCCEEDED(hr) && HrTrace(hr));
#else
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA subresource_data;
			subresource_data.pSysMem = &data;
			subresource_data.SysMemPitch = 0;
			subresource_data.SysMemSlicePitch = 0;
			hr = device->CreateBuffer(&buffer_desc, &subresource_data, buffer_object.GetAddressOf());
			assert(SUCCEEDED(hr) && HrTrace(hr));
#endif
		}
		virtual ~ConstantBuffer() = default;
		ConstantBuffer(ConstantBuffer&) = delete;
		ConstantBuffer& operator =(ConstantBuffer&) = delete;

		//バッファの更新 usageからセットするシェーダーを選択
		void Activate(int slot, CBufferUsage usage)
		{
			Activate(slot, usage, &data_);
		}
		void Activate(int slot, CBufferUsage usage, const T* data_)
		{
			ID3D11DeviceContext* deviceContext = DXSystem::deviceContext_.Get();

				HRESULT hr = S_OK;
#ifdef USAGE_DYNAMIC
			D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
			D3D11_MAPPED_SUBRESOURCE mappedBuffer;

			hr = deviceContext->Map(bufferObject_.Get(), 0, map, 0, &mappedBuffer);
			assert(SUCCEEDED(hr) && HrTrace(hr));
			memcpy_s(mappedBuffer.pData, sizeof(T), data_, sizeof(T));
			deviceContext->Unmap(bufferObject_.Get(), 0);
#else
			immediate_context->UpdateSubresource(buffer_object.Get(), 0, 0, &data, 0, 0);
#endif
			if (static_cast<byte>(usage) & static_cast<byte>(CBufferUsage::v))
			{
				deviceContext->VSSetConstantBuffers(slot, 1, bufferObject_.GetAddressOf());
			}
			if (static_cast<byte>(usage) & static_cast<byte>(CBufferUsage::h))
			{
				deviceContext->HSSetConstantBuffers(slot, 1, bufferObject_.GetAddressOf());
			}
			if (static_cast<byte>(usage) & static_cast<byte>(CBufferUsage::d))
			{
				deviceContext->DSSetConstantBuffers(slot, 1, bufferObject_.GetAddressOf());
			}
			if (static_cast<byte>(usage) & static_cast<byte>(CBufferUsage::g))
			{
				deviceContext->GSSetConstantBuffers(slot, 1, bufferObject_.GetAddressOf());
			}
			if (static_cast<byte>(usage) & static_cast<byte>(CBufferUsage::p))
			{
				deviceContext->PSSetConstantBuffers(slot, 1, bufferObject_.GetAddressOf());
			}
			if (static_cast<byte>(usage) & static_cast<byte>(CBufferUsage::c))
			{
				deviceContext->CSSetConstantBuffers(slot, 1, bufferObject_.GetAddressOf());
			}
		}
		void Deactivate()
		{
			// NOP
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> bufferObject_;

	};
}