#include "ComputeParticle.hlsli"

RWStructuredBuffer<ParticleData> particleDataBuffer : register(u0); //  パーティクル管理バッファ
AppendStructuredBuffer<uint> particleUnusedBuffer : register(u1); //  パーティクル番号管理バッファ(末尾への追加専用)

RWByteAddressBuffer indirectDataBuffer : register(u2); //インダイレクト用バッファー

[numthreads(Num_Particle_Thread, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint index = dispatchThreadId.x;

    //  有効フラグが立っているものだけ処理
    if (particleDataBuffer[index].parameter.z < 0.0f)return;
    
    float delta = deltaTime * particleDataBuffer[index].parameter.w;

    //  経過時間分減少させる
    particleDataBuffer[index].parameter.y -= deltaTime;
    if (particleDataBuffer[index].parameter.y < 0)
    {
    //  寿命が付きたら未使用リストに追加
        particleDataBuffer[index].parameter.z = -1.0f; //  生存フラグを初期化しておく
        particleUnusedBuffer.Append(index);
        
        //死亡数をカウントする
        indirectDataBuffer.InterlockedAdd(Indirect_Arguments_Num_Dead_Particle, 1);
        
        return;
    }

    //  速度更新
    particleDataBuffer[index].velocity.xyz += particleDataBuffer[index].acceleration.xyz * deltaTime;

    //  位置更新
    particleDataBuffer[index].position.xyz += particleDataBuffer[index].velocity.xyz * deltaTime;

    // 回転速度更新
    particleDataBuffer[index].rotationVeclocity.xyz += particleDataBuffer[index].rotationAcceleration.xyz * deltaTime;
    
    //回転更新
    particleDataBuffer[index].rotation.xyz += particleDataBuffer[index].rotationVeclocity.xyz * deltaTime;
    
    //スケール速度変更
    particleDataBuffer[index].scaleVeclocity.xyz += particleDataBuffer[index].scaleAcceleration.xyz * deltaTime;
    
    //回転更新
    particleDataBuffer[index].scale.xyz += particleDataBuffer[index].scaleVeclocity.xyz * deltaTime;
    
    //  切り取り座標を算出
    uint type = (uint) (particleDataBuffer[index].parameter.x + 0.5f);
    
    float w = 1.0 / textureSplitCount.x;
    float h = 1.0 / textureSplitCount.y;
    
    float2 uv = float2((type % textureSplitCount.x) * w, (type / textureSplitCount.x) * h);
    particleDataBuffer[index].texcoord.xy = uv;
    particleDataBuffer[index].texcoord.zw = float2(w, h);

    //徐々に透明にしていく
    particleDataBuffer[index].color.a = saturate(particleDataBuffer[index].parameter.y);
}