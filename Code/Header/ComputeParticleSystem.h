#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "MathHelper.h"

#include <vector>

//#include "misc.h"
//#include "texture.h"
//#include "shader.h"

namespace AbyssEngine
{
    class ComputeParticleSystem
    {
    public:
        //パーティクルスレッド数
        static constexpr UINT Num_Particle_Thread = 1024;

        //パーティクル生成用構造体
        struct EmitParticleData
        {
            Vector4	parameter_;						//	x : パーティクル処理タイプ, y : 生存時間, z、w : 経過時間倍率

            Vector4	position_          = { 0, 0, 0, 0 };//	生成座標
            Vector4	rotation_          = { 0, 0, 0, 0 };//	拡縮情報
            Vector4	scale_ = { 1, 1, 1, 0 };//	回転情報

            Vector4	velocity_          = { 0, 0, 0, 0 };//	初速
            Vector4	acceleration_      = { 0, 0, 0, 0 };//	加速度

            Vector4 rotationVelocity_    = { 0, 0, 0, 0 };//　回転初速
            Vector4 rotationAcceleration_ = { 0, 0, 0, 0 };//　回転加速度

            Vector4 scaleVelocity_       = { 0, 0, 0, 0 };//　スケール初速
            Vector4 scaleAcceleration_    = { 0, 0, 0, 0 };//　スケール加速度

            Vector4	color_             = { 1, 1, 1, 1 };//	色情報
        };


        //  パーティクル構造体
        //	アプリケーション側では使用しないが、形式として必要なためここで宣言しておく
        struct ParticleData
        {
            Vector4	parameter_;		//	x : パーティクル処理タイプ, y : 生存時間, z : 生存フラグ, w : 経過時間倍率

            Vector4	position_;		//	生成座標
            Vector4	rotation_;		//	拡縮情報
            Vector4	scale_;			//	回転情報

            Vector4	velocity_;		//	初速
            Vector4	acceleration_;	//	加速度

            Vector4 rotationVelocity_; //　回転初速
            Vector4 rotationAcceleration_ ;//　回転加速度

            Vector4 scaleVelocity_; //　スケール初速
            Vector4 scaleAcceleration_;//　スケール加速度

            Vector4	texcoord;		//	UV座標
            Vector4	color;			//	色情報
        };

        //汎用情報定義
        struct CommonConstants
        {
            float deltaTime_;
            DirectX::XMUINT2 textureSplitCount_; //テクスチャの分割数
            UINT systemNumParticles_;   //パーティクル総数

            UINT totalEmitCount_; //　現在のフレームでのパーティクル総生成数
            UINT commonDummy_[3];
        };

        //DispatchIndirect用構造体
        using DispatchIndirect = DirectX::XMUINT3; //UINT3で十分

        // 00バイト目：現在のパーティクル総数
        // 04バイト目：１F前のパーティクル総数
        // 08バイト目：パーティクル破棄数
        // 12バイト目：パーティクル生成用DispatchIndirect情報
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

        const DirectX::XMUINT2& GetTextureSplitCount() const { return textureSplitCount_; }

    private:
        UINT numParticles_;
        UINT numEmitParticles_;
        bool oneShotInitialize_;
        DirectX::XMUINT2 textureSplitCount_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;

        std::vector<EmitParticleData> emitParticles_;
        Microsoft::WRL::ComPtr<ID3D11Buffer> commonContantBuffer_;

        //パーティクルバッファ
        Microsoft::WRL::ComPtr<ID3D11Buffer> particleDataBuffer_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleDataShaderResourceView_;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleDataUnorderedAccessView_;

        //未使用パーティクル番号を格納したAppend/Cosumeバッファ
        Microsoft::WRL::ComPtr<ID3D11Buffer> particleAppendConsumeBuffer_;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleAppendConsumeUnorderedAccessView_;

        //パーティクル生成情報を格納したバッファ
        Microsoft::WRL::ComPtr<ID3D11Buffer> particleEmitBuffer_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleEmitShaderResourceView_;

        //各種シェーダー
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> initShader_;
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> emitShader_;
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> updateShader_;

        Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;

        //DrawIndirectを用いるため、RWStructureBufferを用いる物に変更
        Microsoft::WRL::ComPtr<ID3D11Buffer> indirectDataBuffer_;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> indirectDataUnorderedAccessView_;

        Microsoft::WRL::ComPtr<ID3D11ComputeShader> beginFrameShader_;
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> endFrameShader_;
    };
}

