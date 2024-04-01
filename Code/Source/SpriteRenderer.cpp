#include "SpriteRenderer.h"
#include "Texture.h"
#include "Transform.h"
#include "Engine.h"
#include "RenderManager.h"
#include "Actor.h"

using namespace std;
using namespace AbyssEngine;
using namespace DirectX;

void SpriteRenderer::Initialize(const shared_ptr<Actor>& actor)
{
	//�}�l�[�W���[�̓o�^�Ə�����
	actor_ = actor;
	transform_ = actor->GetTransfrom();

    const Vertex v[] = {
        Vector3(-0.5f, 0.5f,0),	Vector2(0,0),Vector4(1,1,1,1), //����
        Vector3( 0.5f, 0.5f,0),	Vector2(1,0),Vector4(1,1,1,1), //�E��
        Vector3(-0.5f,-0.5f,0),	Vector2(0,1),Vector4(1,1,1,1), //����
        Vector3( 0.5f,-0.5f,0),	Vector2(1,1),Vector4(1,1,1,1), //�E��
    };

    //���_�o�b�t�@�쐬
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(v);
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA res;
    ZeroMemory(&res, sizeof(res));
    res.pSysMem = v;
	res.SysMemPitch = 0;
	res.SysMemSlicePitch = 0;
    auto hr = DXSystem::device_->CreateBuffer(&bd, &res, vertexBuffer_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
    
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	vertexShader_ = Shader<ID3D11VertexShader>::Emplace("./Resources/Shader/SpriteVS.cso", inputLayout_.GetAddressOf(),inputElementDesc, ARRAYSIZE(inputElementDesc));
	pixelShader_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/SpritePS.cso");

    //�e�N�X�`���ǂݍ���
    if (filePath_.empty())
    {
        _ASSERT_EXPR(false, L"�t�@�C���p�X����ł�");
    }
    else
    {
        texture_ = Texture::Load(filePath_);
    }

	size_ = Vector2(texture_->GetWidth(), texture_->GetHeight());

	canRender_ = true;

	SetActive(GetEnabled());
}

void SpriteRenderer::Render()
{
    if (!canRender_)return;

    //��\���̏ꍇ�������Ȃ�
    if (!visibility_)return;

    //���_�o�b�t�@�̎w��
    constexpr UINT stride = sizeof(Vertex);
    constexpr UINT offset = 0;
    DXSystem::deviceContext_->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);

	DXSystem::deviceContext_->IASetInputLayout(inputLayout_.Get());

    //�e�N�X�`���̐ݒ�
    texture_->Set(1, Shader_Type::Pixel);

	DXSystem::deviceContext_->VSSetShader(vertexShader_.Get(), nullptr, 0);
	DXSystem::deviceContext_->PSSetShader(pixelShader_.Get(), nullptr, 0);

    //�`��
    DXSystem::deviceContext_->Draw(4, 0);
}

void SpriteRenderer::RecalculateFrame()
{
    if (!canRender_)return;

    //�\���ؑ�
    if (!visibility_)return;

    //���_�f�[�^�ݒ�
    Vertex data[4];

    const Vector3 transPos = actor_->GetTransfrom()->GetPosition();
    const Vector3 transScale = actor_->GetTransfrom()->GetScale();
    const Vector2 scaledSize = size_ * Vector2(transScale.x,transScale.y);

    data[0].pos_.x = transPos.x;
    data[0].pos_.y = transPos.y;
    data[0].pos_.z = 0.0f;

    data[1].pos_.x = transPos.x + scaledSize.x;
    data[1].pos_.y = transPos.y;
    data[1].pos_.z = 0.0f;

    data[2].pos_.x = transPos.x;
    data[2].pos_.y = transPos.y + scaledSize.y;
    data[2].pos_.z = 0.0f;

    data[3].pos_.x = transPos.x + scaledSize.x;
    data[3].pos_.y = transPos.y + scaledSize.y;
    data[3].pos_.z = 0.0f;

	// ���S���W�����_��
	const float mx = transPos.x + scaledSize.x * 0.5f;
	const float my = transPos.y + scaledSize.y * 0.5f;
	data[0].pos_.x -= mx; data[0].pos_.y -= my;
	data[1].pos_.x -= mx; data[1].pos_.y -= my;
	data[2].pos_.x -= mx; data[2].pos_.y -= my;
	data[3].pos_.x -= mx; data[3].pos_.y -= my;

	const float z = transform_->GetEulerAngles().z;
	const float cos = cosf(XMConvertToRadians(z));
	const float sin = sinf(XMConvertToRadians(z));

	float rx = data[0].pos_.x;
	float ry = data[0].pos_.y;
	data[0].pos_.x = cos * rx + -sin * ry;
	data[0].pos_.y = sin * rx + cos * ry;

	rx = data[1].pos_.x;
	ry = data[1].pos_.y;
	data[1].pos_.x = cos * rx + -sin * ry;
	data[1].pos_.y = sin * rx + cos * ry;

	rx = data[2].pos_.x;
	ry = data[2].pos_.y;
	data[2].pos_.x = cos * rx + -sin * ry;
	data[2].pos_.y = sin * rx + cos * ry;

	rx = data[3].pos_.x;
	ry = data[3].pos_.y;
	data[3].pos_.x = cos * rx + -sin * ry;
	data[3].pos_.y = sin * rx + cos * ry;

	// �����������߂�
	data[0].pos_.x += mx; data[0].pos_.y += my;
	data[1].pos_.x += mx; data[1].pos_.y += my;
	data[2].pos_.x += mx; data[2].pos_.y += my;
	data[3].pos_.x += mx; data[3].pos_.y += my;

	// ���K���f�o�C�X���W�n
	for (auto& i : data)
	{
		i.pos_.x = 2.0f * i.pos_.x / 1920.0f - 1.0f;
		i.pos_.y = 1.0f - 2.0f * i.pos_.y / 1080.0f;
		i.pos_.z = 0.0f;
	}

	//�e�N�X�`�����W�ݒ�
	/*data[0].tex_.x = uvOrigin_.x;
	data[0].tex_.y = uvOrigin_.y;
	data[1].tex_.x = uvOrigin_.x + uvSize_.x;
	data[1].tex_.y = uvOrigin_.y;
	data[2].tex_.x = uvOrigin_.x;
	data[2].tex_.y = uvOrigin_.y + uvSize_.y;
	data[3].tex_.x = uvOrigin_.x + uvSize_.x;
	data[3].tex_.y = uvOrigin_.y + uvSize_.y;*/

	data[0].tex_.x = 0;
	data[0].tex_.y = 0;
	data[1].tex_.x = 1;
	data[1].tex_.y = 0;
	data[2].tex_.x = 0;
	data[2].tex_.y = 1;
	data[3].tex_.x = 1;
	data[3].tex_.y = 1;

	//UV���W
	/*const float w = static_cast<float>(texture_->GetWidth());
	const float h = static_cast<float>(texture_->GetHeight());
	for (auto& i : data)
	{
		i.tex_.x = i.tex_.x / w;
		i.tex_.y = i.tex_.y / h;
	}*/
	//���_�J���[
	data[0].color_ = color_;
	data[1].color_ = color_;
	data[2].color_ = color_;
	data[3].color_ = color_;


	constexpr UINT subresourceIndex = 0;
	D3D11_MAPPED_SUBRESOURCE mapped;
	const auto hr = DXSystem::deviceContext_->Map(vertexBuffer_.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, data, sizeof(data));
		DXSystem::deviceContext_->Unmap(vertexBuffer_.Get(), subresourceIndex);
	}
}

void SpriteRenderer::SetActive(const bool value)
{
	if (value)
	{
		if (!isCalled_)
		{
			Engine::renderManager_->Add(static_pointer_cast<SpriteRenderer>(shared_from_this()));
			isCalled_ = true;
		}
	}
}
