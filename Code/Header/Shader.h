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
            fopen_s(&fp, name, "rb");
            _ASSERT_EXPR(fp, L"cso file not found");

            fseek(fp, 0, SEEK_END);
            size_ = ftell(fp);
            fseek(fp, 0, SEEK_SET);

            data_ = std::make_unique<unsigned char[]>(size_);
            fread(data_.get(), size_, 1, fp);
            fclose(fp);
        }
    };

    //各種シェーダーの生成関数
    template <class T>
    HRESULT CreateShader(ID3D11Device* device, const Blob& cso, T** pixel_shader);
    template <>
    HRESULT CreateShader<ID3D11PixelShader>(ID3D11Device* device, const Blob& cso, ID3D11PixelShader** shader);
    template <>
    HRESULT CreateShader<ID3D11HullShader>(ID3D11Device* device, const Blob& cso, ID3D11HullShader** shader);
    template <>
    HRESULT CreateShader<ID3D11DomainShader>(ID3D11Device* device, const Blob& cso, ID3D11DomainShader** shader);
    template <>
    HRESULT CreateShader<ID3D11GeometryShader>(ID3D11Device* device, const Blob& cso, ID3D11GeometryShader** shader);
    template <>
    HRESULT CreateShader<ID3D11ComputeShader>(ID3D11Device* device, const Blob& cso, ID3D11ComputeShader** shader);

    //各種シェーダークラス
    template <class T>
    class Shader
    {
    private:
        static std::unordered_map<std::string, Microsoft::WRL::ComPtr<T>> shaders_;
        static std::mutex mutex_;

    public:

        //マップの中にある指定されたシェーダーを返す　一度も使用していないシェーダーならマップに追加した後に返す
        static Microsoft::WRL::ComPtr<T> Emplace(const char* name)
        {
            Microsoft::WRL::ComPtr<T> shader;
            if (shaders_.find(name) != shaders_.end())
            {
                shader = shaders_.at(name);
            }
            else
            {
                Blob cso(name);
                HRESULT hr = CreateShader<T>(DXSystem::device_.Get(), cso, shader.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

                std::lock_guard<std::mutex> lock(mutex_);
                shaders_.emplace(std::make_pair(name, shader));
            }
            return shader;
        }

        //マップをクリア
        static void Exterminate()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            shaders_.clear();
        }
    };

    template<class T>
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<T>> Shader<T>::shaders_;
    template<class T> 
    std::mutex Shader<T>::mutex_;

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
}


