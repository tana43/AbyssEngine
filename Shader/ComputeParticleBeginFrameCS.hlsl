#include "ComputeParticle.hlsli"

RWByteAddressBuffer indirectDataBuffer : register(u2);

[numthreads(1, 1, 1)]
void main( uint3 dispatchThreadId : SV_DispatchThreadID )
{
    uint index = dispatchThreadId.x;
    
    // 1F前フレームの総パーティクル数 + 現在フレームの生成パーティクル数 = "仮の"現在のフレームの総パーティクル数
    uint previousNumParticle = indirectDataBuffer.Load(Indirect_Arguments_Num_Current_Particle);
    uint currentNumParticle = previousNumParticle + totalEmitCount;
    
    //源座フレームの総パーティクル数はシステムの総パーティクル数で制限
    currentNumParticle = min(systemNumParticles, currentNumParticle);
    
    //総数を記録
    indirectDataBuffer.Store(Indirect_Arguments_Num_Current_Particle, currentNumParticle);
    indirectDataBuffer.Store(Indirect_Arguments_Num_Pravious_Particle, previousNumParticle);

    //死亡カウンターを初期化
    indirectDataBuffer.Store(Indirect_Arguments_Num_Dead_Particle, 0);
    
    //エミッター用のdispatch indirect に起動数を設定する
    uint3 emitDispatch;
    emitDispatch.x = currentNumParticle - previousNumParticle;
    emitDispatch.y = 1;
    emitDispatch.z = 1;
    indirectDataBuffer.Store3(Indirect_Arguments_Emit_Particle_Dispatch_Indirect, emitDispatch);
}