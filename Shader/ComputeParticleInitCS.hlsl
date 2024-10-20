#include "ComputeParticle.hlsli"

RWStructuredBuffer<ParticleData> particleDataBuffer : register(u0); //�p�[�e�B�N���Ǘ��o�b�t�@
AppendStructuredBuffer<int> particleUnusedBuffer : register(u1); //�p�[�e�B�N���ԍ��Ǘ��o�b�t�@�i�����ւ̒ǉ���p�j

[numthreads(Num_Particle_Thread, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    int index = dispatchThreadId.x;
    
    //�p�[�e�B�N����񏉊���
    particleDataBuffer[index].parameter.z = -1.0f;
    
    //���g�p���X�g�iAppendStructuredBuffer�j�̖����ɒǉ�
    particleUnusedBuffer.Append(index);
}