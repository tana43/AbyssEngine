#include "ComputeParticle.hlsli"

RWStructuredBuffer<ParticleData> particleDataBuffer : register(u0); //パーティクル管理バッファ
AppendStructuredBuffer<int> particleUnusedBuffer : register(u1); //パーティクル番号管理バッファ（末尾への追加専用）

[numthreads(Num_Particle_Thread, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    int index = dispatchThreadId.x;
    
    //パーティクル情報初期化
    particleDataBuffer[index].parameter.z = -1.0f;
    
    //未使用リスト（AppendStructuredBuffer）の末尾に追加
    particleUnusedBuffer.Append(index);
}