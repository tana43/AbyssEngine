#include "ComputeParticle.hlsli"

RWStructuredBuffer<ParticleData> particleDataBuffer : register(u0); //パーティクル管理バッファ
ConsumeStructuredBuffer<uint> particlePoolBuffer : register(u1); //パーティクル番号管理バッファ(末尾取り出し専用)
StructuredBuffer<EmitParticleData> emitParticleBuffer : register(t0); //パーティクル生成情報バッファ

[numthreads(1, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    //未使用リストの末尾から未使用パーティクルのインデックスを取得
    uint particleIndex = particlePoolBuffer.Consume();
    uint emitIndex = dispatchThreadId.x;
    
    //パーティクル生成処理
    particleDataBuffer[particleIndex].parameter.x = emitParticleBuffer[emitIndex].parameter.x;
    particleDataBuffer[particleIndex].parameter.y = emitParticleBuffer[emitIndex].parameter.y;
    particleDataBuffer[particleIndex].parameter.z = 1.0f;
    particleDataBuffer[particleIndex].parameter.w = emitParticleBuffer[emitIndex].parameter.w;
    
    particleDataBuffer[particleIndex].position             = emitParticleBuffer[emitIndex].position;
    particleDataBuffer[particleIndex].rotation             = emitParticleBuffer[emitIndex].rotation;
    particleDataBuffer[particleIndex].scale                = emitParticleBuffer[emitIndex].scale;
    particleDataBuffer[particleIndex].velocity             = emitParticleBuffer[emitIndex].velocity;
    particleDataBuffer[particleIndex].acceleration         = emitParticleBuffer[emitIndex].acceleration;
    particleDataBuffer[particleIndex].rotationVeclocity    = emitParticleBuffer[emitIndex].rotationVeclocity;
    particleDataBuffer[particleIndex].rotationAcceleration = emitParticleBuffer[emitIndex].rotationAcceleration;
    particleDataBuffer[particleIndex].scaleVeclocity       = emitParticleBuffer[emitIndex].scaleVeclocity;
    particleDataBuffer[particleIndex].scaleAcceleration    = emitParticleBuffer[emitIndex].scaleAcceleration;
    particleDataBuffer[particleIndex].color                = emitParticleBuffer[emitIndex].color;
}