#include "ComputeParticle.hlsli"

//�g�k�s�񐶐�
float4x4 MatrixScaling(float3 scale)
{
    float4x4 m;
	m._11 = scale.x;
	m._12 = 0.0f;
	m._13 = 0.0f;
    m._14 = 0.0f;
	
    m._21 = 0.0f;
    m._22 = scale.y;
    m._23 = 0.0f;
    m._24 = 0.0f;
	
    m._31 = 0.0f;
    m._32 = 0.0f;
    m._33 = scale.z;
    m._34 = 0.0f;
	
    m._41 = 0.0f;
    m._42 = 0.0f;
    m._43 = 0.0f;
    m._44 = 1.0f;
	
    return m;
}

//��]�s�񐶐�
float4x4 MatrixRotationRollPitchYaw(float3 rotation)
{
    float cp, sp;
    float cy, sy;
    float cr, sr;
    sincos(rotation.x, sp, cp);
    sincos(rotation.y, sy, cy);
    sincos(rotation.z, sr, cr);

    float4x4 m;
    m._11 = cr * cy + sr * sp * sy;
    m._12 = sr * cp;
    m._13 = sr * sp * cy - cr * sy;
    m._14 = 0.0f;

    m._21 = cr * sp * sy - sr * cy;
    m._22 = cr * cp;
    m._23 = sr * sy + cr * sp * cy;
    m._24 = 0.0f;

    m._31 = cp * sy;
    m._32 = -sp;
    m._33 = cp * cy;
    m._34 = 0.0f;

    m._41 = 0.0f;
    m._42 = 0.0f;
    m._43 = 0.0f;
    m._44 = 1.0f;
    return m;
}

//  �ړ��s�񐶐�
float4x4 MatrixTranslation(float3 translation)
{
    float4x4 m;
    m._11 = 1.0f;
    m._12 = 0.0f;
    m._13 = 0.0f;
    m._14 = 0.0f;

    m._21 = 0.0f;
    m._22 = 1.0f;
    m._23 = 0.0f;
    m._24 = 0.0f;

    m._31 = 0.0f;
    m._32 = 0.0f;
    m._33 = 1.0f;
    m._34 = 0.0f;

    m._41 = translation.x;
    m._42 = translation.y;
    m._43 = translation.z;
    m._44 = 1.0f;
    return m;
}

StructuredBuffer<ParticleData> particleDataBuffer : register(t0);//�p�[�e�B�N���Ǘ��o�b�t�@
[maxvertexcount(4)]
void main(point GS_IN gin[1], inout TriangleStream<PS_IN> output)
{
    uint vertexId = gin[0].vertexId;
    
    //�������Ă��Ȃ��ꍇ�̓X�P�[�����O�ɂ��Ă���
    float3 scale = particleDataBuffer[vertexId].parameter.z < 0.0f ? float3(0, 0, 0) :
    particleDataBuffer[vertexId].scale.xyz;
    
    //�r���{�[�h�s�񐶐�(�r���[�s��̋t�s��ł����A�������ړ��l�͂���Ȃ�
    float4x4 billboardMatrix = sceneData.inverseView;
    billboardMatrix._41_42_43 = float3(0, 0, 0);
    billboardMatrix._44 = 1.0f;
    
    //���[���h�s��X�V
    float4x4 scaleMatrix = MatrixScaling(scale);
    float4x4 rotationMatrix = mul(billboardMatrix, MatrixRotationRollPitchYaw(particleDataBuffer[vertexId].rotation.xyz));
    float4x4 translationMatrix = MatrixTranslation(particleDataBuffer[vertexId].position.xyz);
    float4x4 worldMatrix = mul(mul(scaleMatrix, rotationMatrix), translationMatrix);
    float4x4 worldViewProjectionMatrix = mul(worldMatrix, sceneData.viewProjection);

    //�e����擾
    float4 texcoord = particleDataBuffer[vertexId].texcoord;
    float4 color = particleDataBuffer[vertexId].color;
    
    //���_����
    static const float4 vertexPositions[4] =
    {
        float4(-0.5f, -0.5f, 0, 1),
        float4(+0.5f, -0.5f, 0, 1),
        float4(-0.5f, +0.5f, 0, 1),
        float4(+0.5f, +0.5f, 0, 1),
    };
    static const float2 vertexTexcoord[4] =
    {
        float2(0, 0),
        float2(1, 0),
        float2(0, 1),
        float2(1, 1),
    };
    for (uint i = 0; i < 4;i++)
    {
        PS_IN element;
        element.position = mul(vertexPositions[i], worldViewProjectionMatrix);
        element.texcoord = texcoord.xy + texcoord.zw * vertexTexcoord[i];
        element.color = color;
        output.Append(element);
    }
    
    output.RestartStrip();
}