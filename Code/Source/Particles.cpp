#include "Particles.h"
#include "DxSystem.h"
#include "Misc.h"
#include "Shader.h"
#include "Engine.h"

using namespace DirectX;
using namespace AbyssEngine;

ParticleSystem::ParticleSystem(int particleCount) : Max_Particle_Count(particleCount)
{
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Particle) * particleCount);
    bufferDesc.StructureByteStride = sizeof(Particle);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    hr = DXSystem::GetDevice()->CreateBuffer(&bufferDesc, NULL, particleBuffer_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    shaderResourceViewDesc.Buffer.ElementOffset = 0;
    shaderResourceViewDesc.Buffer.NumElements = static_cast<UINT>(particleCount);
    hr = DXSystem::GetDevice()->CreateShaderResourceView(particleBuffer_.Get(), &shaderResourceViewDesc, particleBufferSrv_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;
    unorderedAccessViewDesc.Format = DXGI_FORMAT_UNKNOWN;
    unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    unorderedAccessViewDesc.Buffer.FirstElement = 0;
    unorderedAccessViewDesc.Buffer.NumElements = static_cast<UINT>(particleCount);
    unorderedAccessViewDesc.Buffer.Flags = 0;
    hr = DXSystem::GetDevice()->CreateUnorderedAccessView(particleBuffer_.Get(), &unorderedAccessViewDesc, particleBufferUav_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    bufferDesc.ByteWidth = sizeof(ParticleSystemConstants);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;
    hr = DXSystem::GetDevice()->CreateBuffer(&bufferDesc, nullptr, constantBuffer_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    particleVS_            = Shader<ID3D11VertexShader>::Emplace("./Resources/Shader/ParticleVS.cso", NULL, NULL, 0);
    particlePS_            = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/ParticlePS.cso");
    particleGS_            = Shader<ID3D11GeometryShader>::Emplace("./Resources/Shader/ParticleGS.cso");
    particleCS_            = Shader<ID3D11ComputeShader>::Emplace("./Resources/Shader/IntegrateParticleCS.cso");
    particleInitializerCS_ = Shader<ID3D11ComputeShader>::Emplace("./Resources/Shader/InitializeParticleCS.cso");
}

UINT Align(UINT num, UINT alignment)
{
    return (num + (alignment - 1)) & ~(alignment - 1);
}

void ParticleSystem::Integrate()
{
    auto* dc = DXSystem::GetDeviceContext();

    dc->CSSetUnorderedAccessViews(0, 1, particleBufferUav_.GetAddressOf(), NULL);

    //定数バッファ更新
    particleSystemData_.time_ += Time::deltaTime_;
    particleSystemData_.deltaTime_ = Time::deltaTime_;
    particleSystemData_.Max_Particle_Count = Max_Particle_Count;
    dc->UpdateSubresource(constantBuffer_.Get(), 0, 0, &particleSystemData_,0,0);
    dc->CSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());

    //コンピュートシェーダーセット
    dc->CSSetShader(particleCS_.Get(), NULL, 0);
    
    const UINT threadGrounpCountX = Align(static_cast<UINT>(Max_Particle_Count), NUMTHREADS_X) / NUMTHREADS_X;
    dc->Dispatch(threadGrounpCountX, 1, 1);

    ID3D11UnorderedAccessView* nullUnorderedAccessView{};
    dc->CSSetUnorderedAccessViews(0, 1, &nullUnorderedAccessView, NULL);
}

void ParticleSystem::Initialize()
{
    //初期化処理
    auto* dc = DXSystem::GetDeviceContext();

    dc->CSSetUnorderedAccessViews(0, 1, particleBufferUav_.GetAddressOf(), NULL);

    //定数バッファをリセット
    particleSystemData_.time_ = 0;
    particleSystemData_.deltaTime_ = Time::deltaTime_;
    particleSystemData_.Max_Particle_Count = Max_Particle_Count;
    dc->UpdateSubresource(constantBuffer_.Get(), 0, 0, &particleSystemData_, 0, 0);
    dc->CSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());

    //コンピュートシェーダーセット
    dc->CSSetShader(particleInitializerCS_.Get(), NULL, 0);

    const UINT threadGrounpCountX = Align(static_cast<UINT>(Max_Particle_Count), NUMTHREADS_X) / NUMTHREADS_X;
    dc->Dispatch(threadGrounpCountX, 1, 1);

    ID3D11UnorderedAccessView* nullUnorderedAccessView{};
    dc->CSSetUnorderedAccessViews(0, 1, &nullUnorderedAccessView, NULL);
}

void ParticleSystem::Render()
{
    auto* dc = DXSystem::GetDeviceContext();

    dc->VSSetShader(particleVS_.Get(), NULL, 0);
    dc->PSSetShader(particlePS_.Get(), NULL, 0);
    dc->GSSetShader(particleGS_.Get(), NULL, 0);
    dc->GSSetShaderResources(9, 1, particleBufferSrv_.GetAddressOf());

    dc->UpdateSubresource(constantBuffer_.Get(), 0, 0, &particleSystemData_, 0, 0);
    dc->VSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());
    dc->PSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());
    dc->GSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());

    dc->IASetInputLayout(NULL);
    dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
    dc->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    dc->Draw(static_cast<UINT>(Max_Particle_Count), 0);

    ID3D11ShaderResourceView* nullShaderResourceView{};
    dc->GSSetShaderResources(9, 1, &nullShaderResourceView);
    dc->VSSetShader(NULL, NULL, 0);
    dc->PSSetShader(NULL, NULL, 0);
    dc->GSSetShader(NULL, NULL, 0);
}
