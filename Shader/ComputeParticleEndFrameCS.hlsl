#include "ComputeParticle.hlsli"

RWByteAddressBuffer indirectDataBuffer : register(u2);

[numthreads(1, 1, 1)]
void main()
{
    //　死亡カウンターを取得＆初期化
    uint destroyCounter = indirectDataBuffer.Load(Indirect_Arguments_Num_Dead_Particle);
    indirectDataBuffer.Store(Indirect_Arguments_Num_Dead_Particle, 0);
    
    //現在のフレームでのパーティクル総数を再計算
    uint currentNumParticle = indirectDataBuffer.Load(Indirect_Arguments_Num_Current_Particle);
    indirectDataBuffer.Store(Indirect_Arguments_Num_Current_Particle, currentNumParticle - destroyCounter);

}