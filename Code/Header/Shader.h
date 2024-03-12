#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <mutex>

#include "Misc.h"
#include "DXSystem.h" 

namespace AbyssEngine
{
    //シェーダーの種類 外からシェーダーの種類を指定したいときに使う
    enum class Shader_Type
    {
        Vertex,
        Geometry,
        Pixel,
        Hull,
        Domain
    };

    //CSOファイルのロード
    struct Blob
    {
        std::unique_ptr<unsigned char[]> data_;
        long size_;
        Blob(const char* name)
        {
            FILE* fp = NULL;
            fopen_s(&fp, 0, "rb");
            _ASSERT_EXPR(fp, L"cso file not found");

            fseek(fp, 0, SEEK_END);
            size_ = ftell(fp);
            fseek(fp, 0, SEEK_SET);

            data_ = std::make_unique<unsigned char[]>(size_);
            fread(data_.get(), size_, 1, fp);
            fclose(fp);
        }
    };

    //各種シェーダークラス
    template <class T>
    class Shader
    {
    private:
        static std::unordered_map<std::string, Microsoft::WRL::ComPtr<T>> shaders_;
        static std::mutex mutex_;

    public:

        //マップの中にある指定されたシェーダーを返す　一度も使用していないシェーダーならマップに追加した後に返す
        static Microsoft::WRL::ComPtr<T> Emplace(const char* name);

        //マップをクリア
        static void Exterminate();
        
    };

    //頂点シェーダークラス　インプットレイアウトがあるので他のシェーダーとの互換性がないからこいつだけ別で作る
    template<>
    class Shader<ID3D11VertexShader>
    {
    private:
        static std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>> vertexShaders_;
        static std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>> inputLayouts_;
        static std::mutex mutex_;

    public:
        //マップの中にある指定されたシェーダーを返す　一度も使用していないシェーダーならマップに追加した後に返す
        static Microsoft::WRL::ComPtr<ID3D11VertexShader> Emplace(const char* name, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);
        
        //インプットレイアウト生成
        static HRESULT AutoGenerateInputLayout(const char* name, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElemetDesc, size_t numElements);

        static void Exterminate();
    };

    template<>
    class Shader<ID3D11PixelShader>
    {
    public:
        static HRESULT Create(const Blob& cso, ID3D11PixelShader** pixelShader);
    };

    template<>
    class Shader<ID3D11HullShader>
    {
    public:
        static HRESULT Create(const Blob& cso, ID3D11HullShader** hullShader);
    };

    template<>
    class Shader<ID3D11DomainShader>
    {
    public:
        static HRESULT Create(const Blob& cso, ID3D11DomainShader** domainShader);
    };

    template<>
    class Shader<ID3D11GeometryShader>
    {
    public:
        static HRESULT Create(const Blob& cso, ID3D11GeometryShader** geometryShader);
    };

    template<>
    class Shader<ID3D11ComputeShader>
    {
    public:
        static HRESULT Create(const Blob& cso, ID3D11ComputeShader** computeShader);
    };

    inline HRESULT Shader<ID3D11PixelShader>::Create(const Blob& cso, ID3D11PixelShader** pixelShader)
    {
        return DXSystem::device_->CreatePixelShader(cso.data_.get(),cso.size_,NULL,pixelShader);
    }

    inline HRESULT Shader<ID3D11HullShader>::Create(const Blob& cso, ID3D11HullShader** hullShader)
    {
        return DXSystem::device_->CreateHullShader(cso.data_.get(),cso.size_,NULL,hullShader);
    }

    inline HRESULT Shader<ID3D11DomainShader>::Create(const Blob& cso, ID3D11DomainShader** domainShader)
    {
        return DXSystem::device_->CreateDomainShader(cso.data_.get(), cso.size_, NULL, domainShader);
    }

    inline HRESULT Shader<ID3D11GeometryShader>::Create(const Blob& cso, ID3D11GeometryShader** geometryShader)
    {
        return DXSystem::device_->CreateGeometryShader(cso.data_.get(),cso.size_,NULL,geometryShader);
    }

    inline HRESULT Shader<ID3D11ComputeShader>::Create(const Blob& cso, ID3D11ComputeShader** computeShader)
    {
        return DXSystem::device_->CreateComputeShader(cso.data_.get(),cso.size_,NULL,computeShader);
    }
}


