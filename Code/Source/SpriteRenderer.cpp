#include "SpriteRenderer.h"
#include "DXSystem.h"
#include "Texture.h"

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
    //�\���ؑ�
    if (!visibility_)return;

    //���_�f�[�^�ݒ�
    Vertex data[4];

    const XMFLOAT3 pos;
    const XMFLOAT3 scale;
    const XMFLOAT2 scaledSize;
}
