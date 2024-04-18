#pragma once
#include "Shader.h"
#include "Asset.h"

namespace AbyssEngine
{
    //テクスチャ
    class Texture : public Asset
    {
    public:
        static HRESULT LoadTextureFromMemory(const void* data, size_t size,
            ID3D11ShaderResourceView** shaderResourceView);
        static HRESULT LoadTextureFromFile(const std::string& texturePath,ID3D11ShaderResourceView** shaderResourceView, 
            D3D11_TEXTURE2D_DESC* texture2dDesc);
        static HRESULT MakeDummyTexture(ID3D11ShaderResourceView** shaderResourceView,
            DWORD value/*0xAABBGGRR*/, UINT dimension);
    public:
        //テクスチャをファイルを指定し読み込む
        static std::shared_ptr<Texture> Load(const std::string& texturePath, const u_int& textureFlg = 0);

        //テクスチャをシェーダーにステージする
        void Set(UINT slot, AbyssEngine::Shader_Type type);

    public:
        //テクスチャリソースを取得する
        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetResource() const { return shaderResourceView_; } 

        //テクスチャの横幅を返す
        [[nodiscard]] UINT GetWidth() const { return texture2dDesc_.Width; }	

        //テクスチャの縦幅を返す
        [[nodiscard]] UINT GetHeight() const { return texture2dDesc_.Height; } 

        //ファイルパスを取得する
        [[nodiscard]] std::string GetPath() const { return path_; }    

    private:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_ = nullptr;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_ = nullptr;
        D3D11_TEXTURE2D_DESC texture2dDesc_ = {}; //テクスチャ情報
        std::string path_;
    };

}

