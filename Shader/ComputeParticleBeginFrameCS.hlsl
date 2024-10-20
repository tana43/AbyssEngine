#include "ComputeParticle.hlsli"

RWByteAddressBuffer indirectDataBuffer : register(u2);

[numthreads(1, 1, 1)]
void main( uint3 dispatchThreadId : SV_DispatchThreadID )
{
    uint index = dispatchThreadId.x;
    
    // 1F�O�t���[���̑��p�[�e�B�N���� + ���݃t���[���̐����p�[�e�B�N���� = "����"���݂̃t���[���̑��p�[�e�B�N����
    uint previousNumParticle = indirectDataBuffer.Load(Indirect_Arguments_Num_Current_Particle);
    uint currentNumParticle = previousNumParticle + totalEmitCount;
    
    //�����t���[���̑��p�[�e�B�N�����̓V�X�e���̑��p�[�e�B�N�����Ő���
    currentNumParticle = min(systemNumParticles, currentNumParticle);
    
    //�������L�^
    indirectDataBuffer.Store(Indirect_Arguments_Num_Current_Particle, currentNumParticle);
    indirectDataBuffer.Store(Indirect_Arguments_Num_Pravious_Particle, previousNumParticle);

    //���S�J�E���^�[��������
    indirectDataBuffer.Store(Indirect_Arguments_Num_Dead_Particle, 0);
    
    //�G�~�b�^�[�p��dispatch indirect �ɋN������ݒ肷��
    uint3 emitDispatch;
    emitDispatch.x = currentNumParticle - previousNumParticle;
    emitDispatch.y = 1;
    emitDispatch.z = 1;
    indirectDataBuffer.Store3(Indirect_Arguments_Emit_Particle_Dispatch_Indirect, emitDispatch);
}