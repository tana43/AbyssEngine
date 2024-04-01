#include "Shader.h"
#include <sstream>
#include <d3dcompiler.h>

using namespace AbyssEngine;

//template <class T>
//std::unordered_map<std::string, Microsoft::WRL::ComPtr<T>> Shader<T>::shaders_;
//template <class T>
//std::mutex Shader<T>::mutex_;

Microsoft::WRL::ComPtr<ID3D11VertexShader> Shader<ID3D11VertexShader>::Emplace(const char* name, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements)
{
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
    if (vertexShaders_.find(name) != vertexShaders_.end())
    {
        vertexShader_ = vertexShaders_.at(name);
        if (inputLayout)
        {
            if (inputLayouts_.find(name) != inputLayouts_.end())
            {
                *inputLayout = inputLayouts_.at(name).Get();
                (*inputLayout)->AddRef();
            }
            else
            {
#if 1
                AutoGenerateInputLayout(name,inputLayout,inputElementDesc,numElements);
#else
                _ASSERT_EXPR(FALSE, L"");
#endif // 1
            }
        }
    }
    else
    {
        HRESULT hr;

        Blob cso(name);
        hr = DXSystem::device_->CreateVertexShader(cso.data_.get(),cso.size_,nullptr,vertexShader_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

        std::lock_guard<std::mutex> lock(mutex_);
        vertexShaders_.emplace(std::make_pair(name, vertexShader_));

        if (inputLayout)
        {
            hr = DXSystem::device_->CreateInputLayout(inputElementDesc, numElements, cso.data_.get(), cso.size_, inputLayout);
            _ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
            inputLayouts_.emplace(std::make_pair(name, *inputLayout));
        }
    }
    return vertexShader_;
}

std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>> Shader<ID3D11VertexShader>::vertexShaders_;
std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>> Shader<ID3D11VertexShader>::inputLayouts_;
std::mutex Shader<ID3D11VertexShader>::mutex_;

HRESULT Shader<ID3D11VertexShader>::AutoGenerateInputLayout(const char* name, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, size_t numElements)
{
    if (inputLayouts_.find(name) != inputLayouts_.end())
    {
        *inputLayout = inputLayouts_.at(name).Get();
        (*inputLayout)->AddRef();
        return S_OK;
    }

    std::string source;
    source.append("struct VS_IN\n{\n");
    for (size_t lineNumber = 0; lineNumber < numElements; lineNumber++)
    {
        std::stringstream line;
        switch (inputElementDesc[lineNumber].Format)
        {
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            line << "float4";
            break;
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32B32_UINT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
            line << "uint4";
            break;
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R32G32B32_SINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
            line << "int4";
            break;
        default:
            _ASSERT_EXPR_A(0, u8"これはDXGI_FORMATにサポートされていません");
            break;
        }
        line << " _" << inputElementDesc[lineNumber].SemanticName << " : " << inputElementDesc[lineNumber].SemanticName << " ;\n";
        source.append(line.str());
    }
    source.append("};\nvoid main(VS_IN vin){}\n");

    HRESULT hr = S_OK;
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
    flags |= D3DCOMPILE_DEBUG;
    flags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif // _DEBUG
    Microsoft::WRL::ComPtr<ID3D11VertexShader> dummyVertexShader;
    Microsoft::WRL::ComPtr<ID3DBlob> compiledShaderBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorMessageBlob;
    const char* target = "vs_5_0";
    hr = D3DCompile(source.c_str(), source.length(), 0, 0, 0, "main", target, flags, 0, compiledShaderBlob.GetAddressOf(), errorMessageBlob.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), reinterpret_cast<LPCSTR>(errorMessageBlob->GetBufferPointer()));
    hr = DXSystem::device_->CreateVertexShader(compiledShaderBlob->GetBufferPointer(), compiledShaderBlob->GetBufferSize(), 0, dummyVertexShader.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), reinterpret_cast<LPCSTR>(errorMessageBlob->GetBufferPointer()));
    hr = DXSystem::device_->CreateInputLayout(inputElementDesc, static_cast<UINT>(numElements), compiledShaderBlob->GetBufferPointer(), static_cast<UINT>(compiledShaderBlob->GetBufferSize()), inputLayout);

    std::lock_guard<std::mutex> lock(mutex_);
    inputLayouts_.emplace(std::make_pair(name, *inputLayout));

    return hr;
}

void Shader<ID3D11VertexShader>::Exterminate()
{
    std::lock_guard<std::mutex> lock(mutex_);
    inputLayouts_.clear();
    vertexShaders_.clear();
}

template <>
HRESULT AbyssEngine::CreateShader<ID3D11PixelShader>(ID3D11Device* device, const Blob& cso, ID3D11PixelShader** shader)
{
    return device->CreatePixelShader(cso.data_.get(), cso.size_, NULL, shader);
}
template <>
HRESULT AbyssEngine::CreateShader<ID3D11HullShader>(ID3D11Device* device, const Blob& cso, ID3D11HullShader** shader)
{
    return device->CreateHullShader(cso.data_.get(), cso.size_, NULL, shader);
}
template <>
HRESULT AbyssEngine::CreateShader<ID3D11DomainShader>(ID3D11Device* device, const Blob& cso, ID3D11DomainShader** shader)
{
    return device->CreateDomainShader(cso.data_.get(), cso.size_, NULL, shader);
}
template <>
HRESULT AbyssEngine::CreateShader<ID3D11GeometryShader>(ID3D11Device* device, const Blob& cso, ID3D11GeometryShader** shader)
{
    return device->CreateGeometryShader(cso.data_.get(), cso.size_, NULL, shader);
}
template <>
HRESULT AbyssEngine::CreateShader<ID3D11ComputeShader>(ID3D11Device* device, const Blob& cso, ID3D11ComputeShader** shader)
{
    return device->CreateComputeShader(cso.data_.get(), cso.size_, NULL, shader);
}
