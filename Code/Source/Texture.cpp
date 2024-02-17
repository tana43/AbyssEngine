#include "Texture.h"
#include "DXSystem.h"
#include "Engine.h"
#include "AssetManager.h"
#include "Misc.h"

#include <tchar.h>
#include <locale>
#include <filesystem>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <Shlwapi.h>

using Microsoft::WRL::ComPtr;
using namespace AbyssEngine;
using namespace std;
using namespace DirectX;

std::shared_ptr<Texture> Texture::Load(const std::string& texturePath, const u_int& textureFlg)
{
    HRESULT hr = S_OK;

    setlocale(LC_ALL, "japanese");
    wchar_t fileName[MAX_PATH] = { 0 };
    size_t ret = 0;
    mbstowcs_s(&ret, fileName, MAX_PATH, texturePath.c_str(), _TRUNCATE);

    //すでにロード済のテクスチャか？
    //find()は見つからなかった場合 end()へのイテレータを返す
    const auto it = Engine::assetManager_->cacheTexture_.find(texturePath);
    if (it != Engine::assetManager_->cacheTexture_.end())
    {
        return it->second;
    }

    ComPtr<ID3D11Resource> resource;
    auto texture = make_shared<Texture>();

    HRESULT hr{ S_OK };

    ComPtr<ID3D11ShaderResourceView> shaderResourceView;

    filesystem::path ddsFilename(texturePath);
    ddsFilename.replace_extension("dds");
    if (std::filesystem::exists(ddsFilename.c_str()))
    {
        hr = CreateDDSTextureFromFile(DXSystem::device_.Get(), ddsFilename.c_str(), resource.GetAddressOf(), shaderResourceView.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
    }
    else
    {
        hr = CreateWICTextureFromFile(DXSystem::device_.Get(), fileName, resource.GetAddressOf(), shaderResourceView.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
    }

    texture->shaderResourceView_->GetResource(resource.GetAddressOf());

    //テクスチャ情報取得
    ComPtr<ID3D11Texture2D> texture2d;
    resource.Get()->QueryInterface(texture2d.GetAddressOf());
    texture2d.Get()->GetDesc(&texture->texture2dDesc_);

    //サンプラーステート作成
    D3D11_SAMPLER_DESC sd = {};
    sd.Filter = D3D11_FILTER_ANISOTROPIC; //異方性フィルタ
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // U
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // V
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // W
    sd.MipLODBias = 0;
    sd.MaxAnisotropy = 4;//最大異方性（1ピクセルあたりのテクスチャ点数）
    sd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sd.MinLOD = 0;
    sd.MaxLOD = D3D11_FLOAT32_MAX;

    hr = DXSystem::device_->CreateSamplerState(&sd, texture->sampler_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

    //アセットマネージャーに登録
    Engine::assetManager_->cacheTexture_.insert(make_pair(texturePath, texture));
    Engine::assetManager_->RegistrationAsset(texture);
    texture->path_ = texturePath;
    return texture;
}

void Texture::Set(UINT slot, Shader_Type type)
{
    if (shaderResourceView_)
    {
        switch (type)
        {
        case Shader_Type::Vertex:
            DXSystem::deviceContext_->VSSetShaderResources(slot, 1, shaderResourceView_.GetAddressOf());
            DXSystem::deviceContext_->VSSetSamplers(slot, 1, sampler_.GetAddressOf());
            break;
        case Shader_Type::Geometry:
            DXSystem::deviceContext_->GSSetShaderResources(slot, 1, shaderResourceView_.GetAddressOf());
            DXSystem::deviceContext_->GSSetSamplers(slot, 1, sampler_.GetAddressOf());
            break;
        case Shader_Type::Pixel:
            DXSystem::deviceContext_->PSSetShaderResources(slot, 1, shaderResourceView_.GetAddressOf());
            DXSystem::deviceContext_->PSSetSamplers(slot, 1, sampler_.GetAddressOf());
            break;
        case Shader_Type::Hull:
            DXSystem::deviceContext_->HSSetShaderResources(slot, 1, shaderResourceView_.GetAddressOf());
            DXSystem::deviceContext_->HSSetSamplers(slot, 1, sampler_.GetAddressOf());
            break;
        case Shader_Type::Domain:
            DXSystem::deviceContext_->DSSetShaderResources(slot, 1, shaderResourceView_.GetAddressOf());
            DXSystem::deviceContext_->DSSetSamplers(slot, 1, sampler_.GetAddressOf());
            break;
        }
    }
}
