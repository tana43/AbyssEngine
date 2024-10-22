#include "ComputeParticle.hlsli"

RWStructuredBuffer<ParticleData> particleDataBuffer : register(u0); //�p�[�e�B�N���Ǘ��o�b�t�@
ConsumeStructuredBuffer<uint> particlePoolBuffer : register(u1); //�p�[�e�B�N���ԍ��Ǘ��o�b�t�@(�������o����p)
StructuredBuffer<EmitParticleData> emitParticleBuffer : register(t0); //�p�[�e�B�N���������o�b�t�@

[numthreads(1, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    //���g�p���X�g�̖������疢�g�p�p�[�e�B�N���̃C���f�b�N�X���擾
    uint particleIndex = particlePoolBuffer.Consume();
    uint emitIndex = dispatchThreadId.x;
    
    //�p�[�e�B�N����������
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