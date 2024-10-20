#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

#include <vector>

//#include "misc.h"
//#include "texture.h"
//#include "shader.h"

namespace AbyssEngine
{
    class ComputeParticleSystem
    {
    public:
        //�p�[�e�B�N���X���b�h��
        static constexpr UINT Num_Particle_Thread = 1024;

        //�p�[�e�B�N�������p�\����
        struct EmitParticleData
        {
            DirectX::XMFLOAT4	parameter_;						//	x : �p�[�e�B�N�������^�C�v, y : ��������, zw : ��

            DirectX::XMFLOAT4	position_ = { 0, 0, 0, 0 };		//	�������W
            DirectX::XMFLOAT4	rotation_ = { 0, 0, 0, 0 };		//	�g�k���
            DirectX::XMFLOAT4	scale_ = { 1, 1, 1, 0 };			//	��]���

            DirectX::XMFLOAT4	velocity_ = { 0, 0, 0, 0 };		//	����
            DirectX::XMFLOAT4	acceleration_ = { 0, 0, 0, 0 };	//	�����x

            DirectX::XMFLOAT4	color_ = { 1, 1, 1, 1 };			//	�F���
        };


        //  �p�[�e�B�N���\����
        //	�A�v���P�[�V�������ł͎g�p���Ȃ����A�`���Ƃ��ĕK�v�Ȃ��߂����Ő錾���Ă���
        struct ParticleData
        {
            DirectX::XMFLOAT4	parameter_;		//	x : �p�[�e�B�N�������^�C�v, y : ��������, z : �����t���O, w : ��

            DirectX::XMFLOAT4	position_;		//	�������W
            DirectX::XMFLOAT4	rotation_;		//	�g�k���
            DirectX::XMFLOAT4	scale_;			//	��]���

            DirectX::XMFLOAT4	velocity_;		//	����
            DirectX::XMFLOAT4	acceleration_;	//	�����x
            DirectX::XMFLOAT4	texcoord;		//	UV���W
            DirectX::XMFLOAT4	color;			//	�F���
        };

        //�ėp����`
        struct CommonConstants
        {
            float deltaTime_;
            DirectX::XMUINT2 textureSplitCount_; //�e�N�X�`���̕�����
            UINT systemNumParticles_;   //�p�[�e�B�N������

            UINT totalEmitCount_; //�@���݂̃t���[���ł̃p�[�e�B�N����������
            UINT commonDummy_[3];
        };

        //DispatchIndirect�p�\����
        using DispatchIndirect = DirectX::XMUINT3; //UINT3�ŏ\��

        // 00�o�C�g�ځF���݂̃p�[�e�B�N������
        // 04�o�C�g�ځF�PF�O�̃p�[�e�B�N������
        // 08�o�C�g�ځF�p�[�e�B�N���j����
        // 12�o�C�g�ځF�p�[�e�B�N�������pDispatchIndirect���
        static constexpr UINT Num_Current_Paricle_Offset = 0;
        static constexpr UINT Num_Previous_Paricle_Offset = Num_Current_Paricle_Offset + sizeof(UINT);
        static constexpr UINT Num_Dead_Paricle_Offset = Num_Previous_Paricle_Offset + sizeof(UINT);
        static constexpr UINT Emit_Dispatch_Indirect_Offset = Num_Dead_Paricle_Offset + sizeof(UINT);
        static constexpr UINT DrawIndirectSize = Emit_Dispatch_Indirect_Offset + sizeof(DispatchIndirect);

    public:
        ComputeParticleSystem(ID3D11Device* device, UINT particlesCount,
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView,
            DirectX::XMUINT2 splitCount = DirectX::XMUINT2(1, 1));
        ~ComputeParticleSystem();

        void Emit(const EmitParticleData& data);
        void Update(ID3D11DeviceContext* deviceContext, float deltaTime);
        void Render(ID3D11DeviceContext* deviceContext);

    private:
        UINT numParticles_;
        UINT numEmitParticles_;
        bool oneShotInitialize_;
        DirectX::XMUINT2 textureSplitCount_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;

        std::vector<EmitParticleData> emitParticles_;
        Microsoft::WRL::ComPtr<ID3D11Buffer> commonContantBuffer_;

        //�p�[�e�B�N���o�b�t�@
        Microsoft::WRL::ComPtr<ID3D11Buffer> particleDataBuffer_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleDataShaderResourceView_;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleDataUnorderedAccessView_;

        //���g�p�p�[�e�B�N���ԍ����i�[����Append/Cosume�o�b�t�@
        Microsoft::WRL::ComPtr<ID3D11Buffer> particleAppendConsumeBuffer_;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleAppendConsumeUnorderedAccessView_;

        //�p�[�e�B�N�����������i�[�����o�b�t�@
        Microsoft::WRL::ComPtr<ID3D11Buffer> particleEmitBuffer_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleEmitShaderResourceView_;

        //�e��V�F�[�_�[
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> initShader_;
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> emitShader_;
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> updateShader_;

        Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;

        //DrawIndirect��p���邽�߁ARWStructureBuffer��p���镨�ɕύX
        Microsoft::WRL::ComPtr<ID3D11Buffer> indirectDataBuffer_;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> indirectDataUnorderedAccessView_;

        Microsoft::WRL::ComPtr<ID3D11ComputeShader> beginFrameShader_;
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> endFrameShader_;
    };
}

