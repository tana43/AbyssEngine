#include "Constants.hlsli"

//�p�[�e�B�N���X���b�h��
static const int Num_Particle_Thread = 1024;

//�����p�[�e�B�N���\����
struct EmitParticleData
{
    float4 parameter; //x : �p�[�e�B�N�������^�C�v,y : ��������,zw : ��
    
    float4 position;
    float4 rotation;
    float4 scale;
    
    float4 velocity;
    float4 acceleration;
    
    float4 color;
};

//�p�[�e�B�N���\����
struct ParticleData
{
    float4 parameter;//x : �p�[�e�B�N�������^�C�v,y : ��������.z : �����t���O, w : ��
    float4 position;
    float4 rotation;
    float4 scale;
    
    float4 velocity;
    float4 acceleration;
    
    float4 texcoord;
    float4 color;
};

//indiredctDataBuffer�ւ̃A�N�Z�X�p�o�C�g�I�t�Z�b�g
static const uint Indirect_Arguments_Num_Current_Particle = 0;
static const uint Indirect_Arguments_Num_Pravious_Particle = 4;
static const uint Indirect_Arguments_Num_Dead_Particle = 8;
static const uint Indirect_Arguments_Emit_Particle_Dispatch_Indirect = 12;

//�ėp���
cbuffer COMPUTE_PARTICLE_COMMON_CONSTANT_BUFFER : register(b10)
{
    float deltaTime;
    uint2 textureSplitCount;
    uint systemNumParticles;
    
    uint totalEmitCount; //�@�����\��̃p�[�e�B�N����
    uint commonDummy[3];
}

//���_�V�F�[�_�[����W�I���g���V�F�[�_�[�ɓ]��������
struct GS_IN
{
    uint vertexId : VERTEX_ID;
};

//�W�I���g���V�F�[�_�[����s�N�Z���V�F�[�_�[�ɓ]��������
struct PS_IN
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float2 texcoord : TEXCOORD;
};