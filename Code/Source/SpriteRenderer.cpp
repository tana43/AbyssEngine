#include "SpriteRenderer.h"
#include "DXSystem.h"
#include "Texture.h"

using namespace AbyssEngine;
using namespace DirectX;

void SpriteRenderer::Initialize()
{
    constexpr Vertex v[] = {
        XMFLOAT3(-0.5f,0.5f,0),XMFLOAT2(0,0),XMFLOAT4(1,1,1,1), //左上
        XMFLOAT3(0.5f,0.5f,0),XMFLOAT2(1,0),XMFLOAT4(1,1,1,1), //右上
        XMFLOAT3(-0.5f,-0.5f,0),XMFLOAT2(0,1),XMFLOAT4(1,1,1,1), //左下
        XMFLOAT3(0.5f,-0.5f,0),XMFLOAT2(0,0),XMFLOAT4(1,1,1,1), //右下
    };

    //頂点バッファ作成
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

    //テクスチャ読み込み
    if (filePath_.empty())
    {
        _ASSERT_EXPR(false, L"ファイルパスが空です");
    }
    else
    {
        texture_ = Texture::Load(filePath_ + fileName_);
    }
}

void SpriteRenderer::Render()
{
    //非表示の場合処理しない
    if (!visibility_)return;

    //頂点バッファの指定
    constexpr UINT stride = sizeof(Vertex);
    constexpr UINT offset = 0;
    DXSystem::deviceContext_->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);

    //テクスチャの設定
    texture_->Set(1, Shader_Type::Pixel);

    //描画
    DXSystem::deviceContext_->Draw(4, 0);
}

void SpriteRenderer::RecalculateFrame()
{
    //表示切替
    if (!visibility_)return;

    //頂点データ設定
    Vertex data[4];

    const XMFLOAT3 pos;
    const XMFLOAT3 scale;
    const XMFLOAT2 scaledSize;
}
