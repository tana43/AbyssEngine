//  �s�N�Z���V�F�[�_�[�ւ̏o�͗p�\����
struct PSGBufferOut
{
    float4 baseColor : SV_TARGET0;
    float4 emissiveColor : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 parameter : SV_TARGET3;
    float depth : SV_TARGET4;
};

//  GBuffer���\����
struct GBufferData
{
    float4 baseColor; //  �x�[�X�J���[
    float3 emissiveColor; //  ���Ȕ����F
    float3 wNormal; //  ���[���h�@��
    float3 wPosition; //  ���[���h���W(Decode���̂�)
    float depth; //  �[�x
    float roughness; //  �e��
    float metalness; //  ������
    float occlusionFactor; //  �Օ��l
    float occlusionStrength; //  �Օ����x
};

//  GBufferData�ɓZ�߂������s�N�Z���V�F�[�_�[�̏o�͗p�\���̂ɕϊ�
PSGBufferOut EncodeGBuffer(in GBufferData data, matrix view_projection_matrix)
{
    PSGBufferOut ret = (PSGBufferOut) 0;
    //  �x�[�X�J���[��base_color(RT0)�ɏo��
    ret.baseColor = data.baseColor;
    //  ���Ȕ����F��emissive_color(RT1)�ɏo��
    ret.emissiveColor.rgb = data.emissiveColor;
    ret.emissiveColor.a = 1;
    //  �@����normal(RT2)�ɏo��
    ret.normal.rgb = data.wNormal;
    ret.normal.a = 1;
    //  �e��/������/�Օ���parameter(RT3)�ɏo��
    ret.parameter.r = data.occlusionFactor;
    ret.parameter.g = data.roughness;
    ret.parameter.b = data.metalness;
    ret.parameter.a = data.occlusionStrength;
    //  �[�x��depth(RT4)�ɏo��
    float4 position = mul(float4(data.wPosition, 1.0f), view_projection_matrix);
    ret.depth = position.z / position.w;
    return ret;
};
