#include "Texture.h"
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

HRESULT Texture::LoadTextureFromMemory(const void* data, size_t size, ID3D11ShaderResourceView** shaderResourceView)
{
    HRESULT hr = S_OK;
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;

    auto* device = DXSystem::device_.Get();
    hr = CreateDDSTextureFromMemory(device,reinterpret_cast<const uint8_t*>(data),
        size, resource.GetAddressOf(), shaderResourceView);
    if (hr != S_OK)
    {
        hr = CreateWICTextureFromMemory(device, reinterpret_cast<const uint8_t*>(data),
            size, resource.GetAddressOf(), shaderResourceView);
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    return hr;
}

HRESULT Texture::LoadTextureFromFile(const std::string& texturePath, ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2dDesc)
{
    HRESULT hr{ S_OK };
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;

    const auto it = Engine::assetManager_->cacheTexture_.find(texturePath);
    if (it != Engine::assetManager_->cacheTexture_.end())
    {
        shaderResourceView = it->second->shaderResourceView_.GetAddressOf();
        texture2dDesc = &it->second->texture2dDesc_;
    }
    else
    {
        auto* device = DXSystem::device_.Get();

        std::filesystem::path ddsFilename(texturePath);
        ddsFilename.replace_extension("dds");
        if (std::filesystem::exists(ddsFilename.c_str()))
        {
            hr = DirectX::CreateDDSTextureFromFile(device, ddsFilename.c_str(), resource.GetAddressOf(), shaderResourceView);
            _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
        }
        else
        {
            setlocale(LC_ALL, "japanese");
            wchar_t fileName[MAX_PATH] = { 0 };
            size_t ret = 0;
            mbstowcs_s(&ret, fileName, MAX_PATH, texturePath.c_str(), _TRUNCATE);

            hr = DirectX::CreateWICTextureFromFile(device, fileName, resource.GetAddressOf(), shaderResourceView);
            _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
        }
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    texture2d->GetDesc(texture2dDesc);

    return hr;
}

HRESULT Texture::MakeDummyTexture(ID3D11ShaderResourceView** shaderResourceView, DWORD value, UINT dimension)
{
    HRESULT hr{ S_OK };

    D3D11_TEXTURE2D_DESC texture2dDesc{};
    texture2dDesc.Width = dimension;
    texture2dDesc.Height = dimension;
    texture2dDesc.MipLevels = 1;
    texture2dDesc.ArraySize = 1;
    texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture2dDesc.SampleDesc.Count = 1;
    texture2dDesc.SampleDesc.Quality = 0;
    texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
    texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    size_t texels = dimension * dimension;
    std::unique_ptr<DWORD[]> sysmem{std::make_unique<DWORD[]>(texels)};
    for (size_t i = 0; i < texels; ++i)sysmem[i] = value;

    D3D11_SUBRESOURCE_DATA subresourceData;
    subresourceData.pSysMem = sysmem.get();
    subresourceData.SysMemPitch = sizeof(DWORD) * dimension;

    auto* device = DXSystem::device_.Get();
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    hr = device->CreateTexture2D(&texture2dDesc, &subresourceData, &texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
    shaderResourceViewDesc.Format = texture2dDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc,
        shaderResourceView);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    return hr;
}

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

    filesystem::path ddsFilename(texturePath);
    ddsFilename.replace_extension("dds");
    if (std::filesystem::exists(ddsFilename.c_str()))
    {
        hr = CreateDDSTextureFromFile(DXSystem::device_.Get(), ddsFilename.c_str(), resource.GetAddressOf(), texture->shaderResourceView_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }
    else
    {
        hr = CreateWICTextureFromFile(DXSystem::device_.Get(), fileName, resource.GetAddressOf(), texture->shaderResourceView_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

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

    hr = DXSystem::device_->CreateSamplerState(&sd, texture->samplers_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    //アセットマネージャーに登録
    Engine::assetManager_->cacheTexture_.insert(make_pair(texturePath, texture));
    Engine::assetManager_->RegistrationAsset(texture);
    texture->path_ = texturePath;
    return texture;
}

void Texture::Set(UINT slot, Shader_Type type_)
{
    if (shaderResourceView_)
    {
        switch (type_)
        {
        case Shader_Type::Vertex:
            DXSystem::deviceContext_->VSSetShaderResources(slot, 1, shaderResourceView_.GetAddressOf());
            DXSystem::deviceContext_->VSSetSamplers(slot, 1, samplers_.GetAddressOf());
            break;
        case Shader_Type::Geometry:
            DXSystem::deviceContext_->GSSetShaderResources(slot, 1, shaderResourceView_.GetAddressOf());
            DXSystem::deviceContext_->GSSetSamplers(slot, 1, samplers_.GetAddressOf());
            break;
        case Shader_Type::Pixel:
            DXSystem::deviceContext_->PSSetShaderResources(slot, 1, shaderResourceView_.GetAddressOf());
            DXSystem::deviceContext_->PSSetSamplers(slot, 1, samplers_.GetAddressOf());
            break;
        case Shader_Type::Hull:
            DXSystem::deviceContext_->HSSetShaderResources(slot, 1, shaderResourceView_.GetAddressOf());
            DXSystem::deviceContext_->HSSetSamplers(slot, 1, samplers_.GetAddressOf());
            break;
        case Shader_Type::Domain:
            DXSystem::deviceContext_->DSSetShaderResources(slot, 1, shaderResourceView_.GetAddressOf());
            DXSystem::deviceContext_->DSSetSamplers(slot, 1, samplers_.GetAddressOf());
            break;
        }
    }
}
