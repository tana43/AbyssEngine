#pragma once
#include "Shader.h"
#include "Asset.h"

namespace AbyssEngine
{
    //�e�N�X�`��
    class Texture : public Asset
    {
    public:
        //�e�N�X�`�����t�@�C�����w�肵�ǂݍ���
        static std::shared_ptr<Texture> Load(const std::string& texturePath, const u_int& textureFlg = 0);

        //�e�N�X�`�����V�F�[�_�[�ɃX�e�[�W����
        void Set(UINT slot, AbyssEngine::Shader_Type type);

    public:
        //�e�N�X�`�����\�[�X���擾����
        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetResource() const { return shaderResourceView_; } 

        //�e�N�X�`���̉�����Ԃ�
        [[nodiscard]] UINT GetWidth() const { return texture2dDesc_.Width; }	

        //�e�N�X�`���̏c����Ԃ�
        [[nodiscard]] UINT GetHeight() const { return texture2dDesc_.Height; } 

        //�t�@�C���p�X���擾����
        [[nodiscard]] std::string GetPath() const { return path_; }    

    private:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_ = nullptr;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_ = nullptr;
        D3D11_TEXTURE2D_DESC texture2dDesc_ = {}; //�e�N�X�`�����
        std::string path_;
    };

}

