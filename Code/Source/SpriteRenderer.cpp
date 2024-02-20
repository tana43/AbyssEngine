#include "SpriteRenderer.h"
#include "DXSystem.h"
#include "Texture.h"
#include "MathHelper.h"
#include "Actor.h"
#include "Transform.h"
#include "Engine.h"

using namespace AbyssEngine;
using namespace DirectX;

void SpriteRenderer::Initialize()
{
    constexpr Vertex v[] = {
        XMFLOAT3(-0.5f,0.5f,0),XMFLOAT2(0,0),XMFLOAT4(1,1,1,1), //����
        XMFLOAT3(0.5f,0.5f,0),XMFLOAT2(1,0),XMFLOAT4(1,1,1,1), //�E��
        XMFLOAT3(-0.5f,-0.5f,0),XMFLOAT2(0,1),XMFLOAT4(1,1,1,1), //����
        XMFLOAT3(0.5f,-0.5f,0),XMFLOAT2(0,0),XMFLOAT4(1,1,1,1), //�E��
    };

    //���_�o�b�t�@�쐬
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(v);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA res;
    ZeroMemory(&res, sizeof(res));
    res.pSysMem = v;

    DXSystem::device_->CreateBuffer(&bd, &res, vertexBuffer_.GetAddressOf());

    //�e�N�X�`���ǂݍ���
    if (filePath_.empty())
    {
        _ASSERT_EXPR(false, L"�t�@�C���p�X����ł�");
    }
    else
    {
        texture_ = Texture::Load(filePath_ + fileName_);
    }
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

    //�e�N�X�`���̐ݒ�
    texture_->Set(1, Shader_Type::Pixel);

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

    const Vector3 transPos = actor_->transform_->GetPosition();
    const Vector3 transScale = actor_->transform_->GetScale();
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

	const float z = transform_->Get_Euler_Angles().z;
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
		i.pos_.x = 2.0f * i.pos_.x / Engine::render_manager->game_texture->screen_x - 1.0f;
		i.pos_.y = 1.0f - 2.0f * i.pos_.y / Engine::render_manager->game_texture->screen_y;
		i.pos_.z = 0.0f;
	}

	//�e�N�X�`�����W�ݒ�
	data[0].tex.x = uv_origin.x;
	data[0].tex.y = uv_origin.y;
	data[1].tex.x = uv_origin.x + uv_size.x;
	data[1].tex.y = uv_origin.y;
	data[2].tex.x = uv_origin.x;
	data[2].tex.y = uv_origin.y + uv_size.y;
	data[3].tex.x = uv_origin.x + uv_size.x;
	data[3].tex.y = uv_origin.y + uv_size.y;

	//UV���W
	const float w = static_cast<float>(texture->Get_Width());
	const float h = static_cast<float>(texture->Get_Height());
	for (auto& i : data)
	{
		i.tex.x = i.tex.x / w;
		i.tex.y = i.tex.y / h;
	}
	//���_�J���[
	data[0].color = color;
	data[1].color = color;
	data[2].color = color;
	data[3].color = color;


	constexpr UINT subresourceIndex = 0;
	D3D11_MAPPED_SUBRESOURCE mapped;
	const auto hr = DxSystem::device_context->Map(vertex_buffer.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, data, sizeof(data));
		DxSystem::device_context->Unmap(vertex_buffer.Get(), subresourceIndex);
	}
}
}
