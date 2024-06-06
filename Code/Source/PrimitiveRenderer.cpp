#include "Misc.h"
#include "Shader.h"
#include "PrimitiveRenderer.h"

using namespace AbyssEngine;

// �R���X�g���N�^
PrimitiveRenderer::PrimitiveRenderer(ID3D11Device* device)
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	// ���_�V�F�[�_�[
	vertexShader = Shader<ID3D11VertexShader>::Emplace(
		"Data/Shader/PrimitiveRendererVS.cso",
		inputLayout.GetAddressOf(),
		inputElementDesc,
		_countof(inputElementDesc));

	// �s�N�Z���V�F�[�_�[
	pixelShader = Shader<ID3D11PixelShader>::Emplace("Data/Shader/PrimitiveRendererPS.cso");

	// �萔�o�b�t�@
	constantBuffer_ = std::make_unique<ConstantBuffer<CbScene>>();

	// ���_�o�b�t�@
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(Vertex) * VertexCapacity;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&desc, nullptr, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}

// ���_�ǉ�
void PrimitiveRenderer::AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color)
{
	Vertex& v = vertices.emplace_back();
	v.position = position;
	v.color = color;
}

// ���`��
void PrimitiveRenderer::DrawAxis(const DirectX::XMFLOAT4X4& transform, const DirectX::XMFLOAT4& color)
{
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
	DirectX::XMFLOAT3 p, x, y, z;
	DirectX::XMStoreFloat3(&p, W.r[3]);
	DirectX::XMStoreFloat3(&x, DirectX::XMVector3Transform(DirectX::XMVectorSet(1, 0, 0, 0), W));
	DirectX::XMStoreFloat3(&y, DirectX::XMVector3Transform(DirectX::XMVectorSet(0, 1, 0, 0), W));
	DirectX::XMStoreFloat3(&z, DirectX::XMVector3Transform(DirectX::XMVectorSet(0, 0, 1, 0), W));
	AddVertex(p, { 1, 0, 0, 1 });
	AddVertex(x, { 1, 0, 0, 1 });
	AddVertex(p, { 0, 1, 0, 1 });
	AddVertex(y, { 0, 1, 0, 1 });
	AddVertex(p, { 0, 0, 1, 1 });
	AddVertex(z, { 0, 0, 1, 1 });
}

// �O���b�h�`��
void PrimitiveRenderer::DrawGrid(int subdivisions, float scale)
{
	int numLines = (subdivisions + 1) * 2;
	int vertexCount = numLines * 2;

	float corner = 0.5f;
	float step = 1.0f / static_cast<float>(subdivisions);

	int index = 0;
	float s = -corner;

	const DirectX::XMFLOAT4 white = DirectX::XMFLOAT4(1, 1, 1, 1);

	// Create vertical lines
	float scaling = static_cast<float>(subdivisions) * scale;
	DirectX::XMMATRIX M = DirectX::XMMatrixScaling(scaling, scaling, scaling);
	DirectX::XMVECTOR V, P;
	DirectX::XMFLOAT3 position;
	for (int i = 0; i <= subdivisions; i++)
	{
		V = DirectX::XMVectorSet(s, 0, corner, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		AddVertex(position, white);

		V = DirectX::XMVectorSet(s, 0, -corner, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		AddVertex(position, white);

		s += step;
	}

	// Create horizontal lines
	s = -corner;
	for (int i = 0; i <= subdivisions; i++)
	{
		V = DirectX::XMVectorSet(corner, 0, s, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		AddVertex(position, white);

		V = DirectX::XMVectorSet(-corner, 0, s, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		AddVertex(position, white);

		s += step;
	}

	// X��
	{
		const DirectX::XMFLOAT4 red = DirectX::XMFLOAT4(1, 0, 0, 1);
		V = DirectX::XMVectorSet(0, 0, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		AddVertex(position, red);

		V = DirectX::XMVectorSet(corner, 0, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		AddVertex(position, red);
	}

	// Y��
	{
		const DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0, 1, 0, 1);
		V = DirectX::XMVectorSet(0, 0, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		AddVertex(position, green);

		V = DirectX::XMVectorSet(0, corner, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		AddVertex(position, green);
	}

	// Z��
	{
		const DirectX::XMFLOAT4 blue = DirectX::XMFLOAT4(0, 0, 1, 1);
		V = DirectX::XMVectorSet(0, 0, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		AddVertex(position, blue);

		V = DirectX::XMVectorSet(0, 0, corner, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		AddVertex(position, blue);
	}
}

// �`����s
void PrimitiveRenderer::Render(
	const DirectX::XMFLOAT4X4& viewProjection,
	D3D11_PRIMITIVE_TOPOLOGY primitiveTopology)
{
	ID3D11DeviceContext* dc = DXSystem::GetDeviceContext();

	// �V�F�[�_�[�ݒ�
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());

	// �萔�o�b�t�@�ݒ�
	dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// �萔�o�b�t�@�X�V
	constantBuffer_->data_.viewProjection = viewProjection;
	constantBuffer_->Activate(0,CBufferUsage::vp);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dc->IASetPrimitiveTopology(primitiveTopology);
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// �`��
	UINT totalVertexCount = static_cast<UINT>(vertices.size());
	UINT start = 0;
	UINT count = (totalVertexCount < VertexCapacity) ? totalVertexCount : VertexCapacity;

	while (start < totalVertexCount)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		HRESULT hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		memcpy(mappedSubresource.pData, &vertices[start], sizeof(Vertex) * count);

		dc->Unmap(vertexBuffer.Get(), 0);

		dc->Draw(count, 0);

		start += count;
		if ((start + count) > totalVertexCount)
		{
			count = totalVertexCount - start;
		}
	}

	vertices.clear();
}
