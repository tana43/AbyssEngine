#include "ComputeParticle.hlsli"

RWStructuredBuffer<ParticleData> particleDataBuffer : register(u0); //  �p�[�e�B�N���Ǘ��o�b�t�@
AppendStructuredBuffer<uint> particleUnusedBuffer : register(u1); //  �p�[�e�B�N���ԍ��Ǘ��o�b�t�@(�����ւ̒ǉ���p)

RWByteAddressBuffer indirectDataBuffer : register(u2); //�C���_�C���N�g�p�o�b�t�@�[

[numthreads(Num_Particle_Thread, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint index = dispatchThreadId.x;

    //  �L���t���O�������Ă�����̂�������
    if (particleDataBuffer[index].parameter.z < 0.0f)return;
    
    float delta = deltaTime * particleDataBuffer[index].parameter.w;

    //  �o�ߎ��ԕ�����������
    particleDataBuffer[index].parameter.y -= deltaTime;
    if (particleDataBuffer[index].parameter.y < 0)
    {
    //  �������t�����疢�g�p���X�g�ɒǉ�
        particleDataBuffer[index].parameter.z = -1.0f; //  �����t���O�����������Ă���
        particleUnusedBuffer.Append(index);
        
        //���S�����J�E���g����
        indirectDataBuffer.InterlockedAdd(Indirect_Arguments_Num_Dead_Particle, 1);
        
        return;
    }

    //  ���x�X�V
    particleDataBuffer[index].velocity.xyz += particleDataBuffer[index].acceleration.xyz * deltaTime;

    //  �ʒu�X�V
    particleDataBuffer[index].position.xyz += particleDataBuffer[index].velocity.xyz * deltaTime;

    // ��]���x�X�V
    particleDataBuffer[index].rotationVeclocity.xyz += particleDataBuffer[index].rotationAcceleration.xyz * deltaTime;
    
    //��]�X�V
    particleDataBuffer[index].rotation.xyz += particleDataBuffer[index].rotationVeclocity.xyz * deltaTime;
    
    //�X�P�[�����x�ύX
    particleDataBuffer[index].scaleVeclocity.xyz += particleDataBuffer[index].scaleAcceleration.xyz * deltaTime;
    
    //��]�X�V
    particleDataBuffer[index].scale.xyz += particleDataBuffer[index].scaleVeclocity.xyz * deltaTime;
    
    //  �؂�����W���Z�o
    uint type = (uint) (particleDataBuffer[index].parameter.x + 0.5f);
    
    float w = 1.0 / textureSplitCount.x;
    float h = 1.0 / textureSplitCount.y;
    
    float2 uv = float2((type % textureSplitCount.x) * w, (type / textureSplitCount.x) * h);
    particleDataBuffer[index].texcoord.xy = uv;
    particleDataBuffer[index].texcoord.zw = float2(w, h);

    //���X�ɓ����ɂ��Ă���
    particleDataBuffer[index].color.a = saturate(particleDataBuffer[index].parameter.y);
}