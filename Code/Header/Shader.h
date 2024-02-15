#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <mutex>

#include "Misc.h"
#include "DXSystem.h" 

namespace AbyssEngine
{
    //�V�F�[�_�[�̎�� �O����V�F�[�_�[�̎�ނ��w�肵�����Ƃ��Ɏg��
    enum class Shader_Type
    {
        Vertex,
        Geometry,
        Pixel,
        Hull,
        Domain
    };

    //CSO�t�@�C���̃��[�h
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

    template <class T>
    HRESULT Create(const Blob& cso, T** pixelShader);
    template <>
    HRESULT Create<ID3D11PixelShader>(const Blob& cso,ID3D11PixelShader** shader);
    template <>
    HRESULT Create<ID3D11HullShader>(const Blob& cso, ID3D11HullShader** shader);
    template <>
    HRESULT Create<ID3D11DomainShader>(const Blob& cso, ID3D11DomainShader** shader);
    template <>
    HRESULT Create<ID3D11GeometryShader>(const Blob& cso, ID3D11GeometryShader** shader);
    template <>
    HRESULT Create<ID3D11ComputeShader>(const Blob& cso, ID3D11ComputeShader** shader);

    //�e��V�F�[�_�[�N���X
    template <class T>
    class Shader
    {
    private:
        static std::unordered_map<std::string, Microsoft::WRL::ComPtr<T>> shaders_;
        static std::mutex mutex_;

    public:

        //�}�b�v�̒��ɂ���w�肳�ꂽ�V�F�[�_�[��Ԃ��@��x���g�p���Ă��Ȃ��V�F�[�_�[�Ȃ�}�b�v�ɒǉ�������ɕԂ�
        static Microsoft::WRL::ComPtr<T> Emplace(const char* name);

        //�}�b�v���N���A
        static void Exterminate();
        
    };

    //���_�V�F�[�_�[�N���X�@�C���v�b�g���C�A�E�g������̂ő��̃V�F�[�_�[�Ƃ̌݊������Ȃ����炱�������ʂō��
    template<>
    class Shader<ID3D11VertexShader>
    {
    private:
        static std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>> vertexShaders_;
        static std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>> inputLayouts_;
        static std::mutex mutex_;

    public:
        //�}�b�v�̒��ɂ���w�肳�ꂽ�V�F�[�_�[��Ԃ��@��x���g�p���Ă��Ȃ��V�F�[�_�[�Ȃ�}�b�v�ɒǉ�������ɕԂ�
        static Microsoft::WRL::ComPtr<ID3D11VertexShader> Emplace(const char* name, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);
        
        //�C���v�b�g���C�A�E�g����
        static HRESULT AutoGenerateInputLayout(const char* name, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElemetDesc, size_t numElements);

        static void Exterminate();
        
    };
    
}


