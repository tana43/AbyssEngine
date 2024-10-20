#include "ComputeParticle.hlsli"

RWByteAddressBuffer indirectDataBuffer : register(u2);

[numthreads(1, 1, 1)]
void main()
{
    //�@���S�J�E���^�[���擾��������
    uint destroyCounter = indirectDataBuffer.Load(Indirect_Arguments_Num_Dead_Particle);
    indirectDataBuffer.Store(Indirect_Arguments_Num_Dead_Particle, 0);
    
    //���݂̃t���[���ł̃p�[�e�B�N���������Čv�Z
    uint currentNumParticle = indirectDataBuffer.Load(Indirect_Arguments_Num_Current_Particle);
    indirectDataBuffer.Store(Indirect_Arguments_Num_Current_Particle, currentNumParticle - destroyCounter);

}