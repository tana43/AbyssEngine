#include "ComputeParticleSystem.h"
#include "Misc.h"
#include "Texture.h"
#include "Shader.h"

using namespace AbyssEngine;

ComputeParticleSystem::ComputeParticleSystem(ID3D11Device* device, UINT particlesCount, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView, DirectX::XMUINT2 splitCount)
{
    HRESULT hr;
    //パーティクル数をスレッド数に合わせて制限
    numParticles_ = ((particlesCount + (Num_Particle_Thread - 1)) / Num_Particle_Thread) * Num_Particle_Thread;
    numEmitParticles_ = min(numParticles_, 10000);//1fでの生成数制限は要調整

    textureSplitCount_ = splitCount;
    oneShotInitialize_ = false;

    //定数バッファ
    {
        D3D11_BUFFER_DESC bd{};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;
        {
            bd.ByteWidth = sizeof(CommonConstants);
            hr = device->CreateBuffer(&bd, nullptr, commonContantBuffer_.GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
        }
    }

    //パーティクルバッファ生成
    {
        D3D11_BUFFER_DESC desc = {};
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        desc.ByteWidth = sizeof(ParticleData) * numParticles_;
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc.StructureByteStride = sizeof(ParticleData);
        desc.Usage = D3D11_USAGE_DEFAULT;
        hr = device->CreateBuffer(&desc, nullptr, particleDataBuffer_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

        hr = device->CreateShaderResourceView(particleDataBuffer_.Get(), nullptr,
            particleDataShaderResourceView_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

        hr = device->CreateUnorderedAccessView(particleDataBuffer_.Get(), nullptr,
            particleDataUnorderedAccessView_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    //	パーティクルの生成/破棄番号をため込むバッファ生成
    {
        D3D11_BUFFER_DESC desc = {};
        desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        desc.ByteWidth = sizeof(UINT) * numParticles_;
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc.StructureByteStride = sizeof(UINT);
        desc.Usage = D3D11_USAGE_DEFAULT;
        hr = device->CreateBuffer(&desc, nullptr, particleAppendConsumeBuffer_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

        //	Append/Consumeを利用する場合はビュー側にフラグを立てる
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = numParticles_;
        uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
        hr = device->CreateUnorderedAccessView(particleAppendConsumeBuffer_.Get(), &uavDesc, 
            particleAppendConsumeUnorderedAccessView_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    //	パーティクルエミット用バッファ生成
    {
        D3D11_BUFFER_DESC desc = {};
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.ByteWidth = sizeof(EmitParticleData) * numEmitParticles_;
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc.StructureByteStride = sizeof(EmitParticleData);
        desc.Usage = D3D11_USAGE_DEFAULT;
        hr = device->CreateBuffer(&desc, nullptr, particleEmitBuffer_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

        hr = device->CreateShaderResourceView(particleEmitBuffer_.Get(), nullptr, particleEmitShaderResourceView_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    //パーティクルの更新・描画数の削減のためのバッファ
    {
        D3D11_BUFFER_DESC desc = {};
        desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        desc.ByteWidth = DrawIndirectSize;
        desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS | D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
        desc.Usage = D3D11_USAGE_DEFAULT;
        hr = device->CreateBuffer(&desc, nullptr, indirectDataBuffer_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

        //Append/Consumeを利用する場合はビュー側にフラグを立てる
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = desc.ByteWidth / sizeof(UINT);
        uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
        hr = device->CreateUnorderedAccessView(indirectDataBuffer_.Get(), &uavDesc, indirectDataUnorderedAccessView_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    //	コンピュートシェーダー読み込み
    initShader_       = Shader<ID3D11ComputeShader>::Emplace("./Resources/Shader/ComputeParticleInitCS.cso");
    emitShader_       = Shader<ID3D11ComputeShader>::Emplace("./Resources/Shader/ComputeParticleEmitCS.cso");
    updateShader_     = Shader<ID3D11ComputeShader>::Emplace("./Resources/Shader/ComputeParticleUpdateCS.cso");
    beginFrameShader_ = Shader<ID3D11ComputeShader>::Emplace("./Resources/Shader/ComputeParticleBeginFrameCS.cso");
    endFrameShader_   = Shader<ID3D11ComputeShader>::Emplace("./Resources/Shader/ComputeParticleEndFrameCS.cso");


    //	描画用情報生成
    this->shaderResourceView_ = shaderResourceView;
    vertexShader_   = Shader<ID3D11VertexShader>::Emplace("./Resources/Shader/ComputeParticleRenderVS.cso", nullptr, nullptr, 0);
    geometryShader_ = Shader<ID3D11GeometryShader>::Emplace("./Resources/Shader/ComputeParticleRenderGS.cso");
    pixelShader_    = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/ComputeParticleRenderPS.cso");
}

ComputeParticleSystem::~ComputeParticleSystem()
{
    initShader_.Reset();
    updateShader_.Reset();
    emitShader_.Reset();
    vertexShader_.Reset();
    geometryShader_.Reset();
    pixelShader_.Reset();
    shaderResourceView_.Reset();
}

void ComputeParticleSystem::Emit(const EmitParticleData& data)
{
    if (emitParticles_.size() >= numEmitParticles_)return;

    emitParticles_.emplace_back(data);
}

void ComputeParticleSystem::Update(ID3D11DeviceContext* deviceContext, float deltaTime)
{
    //定数バッファ設定
    {
        deviceContext->CSSetConstantBuffers(10, 1, commonContantBuffer_.GetAddressOf());

        //定数バッファ更新
        CommonConstants constant;
        constant.deltaTime_ = deltaTime;
        constant.textureSplitCount_ = textureSplitCount_;
        constant.systemNumParticles_ = numParticles_;
        constant.totalEmitCount_ = static_cast<UINT>(emitParticles_.size());
        deviceContext->UpdateSubresource(commonContantBuffer_.Get(), 0, nullptr, &constant, 0, 0);
    }

    //SRV/UAV設定
    {
        deviceContext->CSSetShaderResources(0, 1, particleEmitShaderResourceView_.GetAddressOf());

        ID3D11UnorderedAccessView* uavs[] =
        {
            particleDataUnorderedAccessView_.Get(),
            particleAppendConsumeUnorderedAccessView_.Get(),
            indirectDataUnorderedAccessView_.Get(),
        };
        deviceContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
    }

    //初期化処理
    if (!oneShotInitialize_)
    {
        oneShotInitialize_ = true;
        deviceContext->CSSetShader(initShader_.Get(), nullptr, 0);
        deviceContext->Dispatch(numParticles_ / Num_Particle_Thread, 1, 1);
    }

    //フレーム開始時の処理
    {
        //現在フレームでのパーティクル総数を算出
        //それに合わせて各種設定を行う
        deviceContext->CSSetShader(beginFrameShader_.Get(), nullptr, 0);
        deviceContext->Dispatch(1, 1, 1);
    }


    //エミット処理
    if(!emitParticles_.empty())
    {
        //エミットバッファ更新
        D3D11_BOX writeBox = {};
        writeBox.left = 0;
        writeBox.right = static_cast<UINT>(emitParticles_.size() * sizeof(EmitParticleData));
        writeBox.top = 0;
        writeBox.bottom = 1;
        writeBox.front = 0;
        writeBox.back = 1;
        deviceContext->UpdateSubresource(
            particleEmitBuffer_.Get(), 
            0, 
            &writeBox,
            emitParticles_.data(), 
            static_cast<UINT>(emitParticles_.size() * sizeof(EmitParticleData)),
            0);
        deviceContext->CSSetShader(emitShader_.Get(), nullptr, 0);
        //deviceContext->Dispatch(static_cast<UINT>(emitParticles_.size()), 1, 1);
        deviceContext->DispatchIndirect(indirectDataBuffer_.Get(),Emit_Dispatch_Indirect_Offset);
        emitParticles_.clear();
    }

    //更新処理
    {
        deviceContext->CSSetShader(updateShader_.Get(), nullptr, 0);
        deviceContext->Dispatch(numParticles_ / Num_Particle_Thread, 1, 1);
    }
    
    //フレーム終了時の処理
    {
        // 総パーティクル数を変動させる
        deviceContext->CSSetShader(endFrameShader_.Get(), nullptr, 0);
        deviceContext->Dispatch(1, 1, 1);
    }

    //UAV設定
    {
        ID3D11UnorderedAccessView* uavs[] = { nullptr,nullptr,nullptr,nullptr };
        deviceContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
    }
}

void ComputeParticleSystem::Render(ID3D11DeviceContext* deviceContext)
{
    //点描画設定
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    //シェーダー設定
    deviceContext->VSSetShader(vertexShader_.Get(), nullptr, 0);
    deviceContext->GSSetShader(geometryShader_.Get(), nullptr, 0);
    deviceContext->PSSetShader(pixelShader_.Get(), nullptr, 0);

    //入力レイアウト設定
    deviceContext->IASetInputLayout(nullptr);

    //リソース設定
    deviceContext->PSSetShaderResources(0, 1, shaderResourceView_.GetAddressOf());
    deviceContext->GSSetShaderResources(0, 1, particleDataShaderResourceView_.GetAddressOf());

    //バッファクリア
    ID3D11Buffer* clearBuffer[] = { nullptr };
    UINT strides[] = { 0 };
    UINT offsets[] = { 0 };
    deviceContext->IASetVertexBuffers(0, 1, clearBuffer, strides, offsets);
    deviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

    //パーティクル情報分描画コール
    deviceContext->Draw(numParticles_, 0);

    //シェーダー無効化
    deviceContext->VSSetShader(nullptr, nullptr, 0);
    deviceContext->GSSetShader(nullptr, nullptr, 0);
    deviceContext->PSSetShader(nullptr, nullptr, 0);

    //リソースクリア
    ID3D11ShaderResourceView* clearShaderResourceViews[] = { nullptr };
    deviceContext->PSSetShaderResources(0, 1, clearShaderResourceViews);
    deviceContext->GSSetShaderResources(0, 1, clearShaderResourceViews);

}
