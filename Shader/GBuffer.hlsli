//  ピクセルシェーダーへの出力用構造体
struct PSGBufferOut
{
    float4 baseColor : SV_TARGET0;
    float4 emissiveColor : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 parameter : SV_TARGET3;
    float depth : SV_TARGET4;
};

//  GBuffer情報構造体
struct GBufferData
{
    float4 baseColor; //  ベースカラー
    float3 emissiveColor; //  自己発光色
    float3 wNormal; //  ワールド法線
    float3 wPosition; //  ワールド座標(Decode時のみ)
    float depth; //  深度
    float roughness; //  粗さ
    float metalness; //  金属質
    float occlusionFactor; //  遮蔽値
    float occlusionStrength; //  遮蔽強度
};

//  GBufferDataに纏めた情報をピクセルシェーダーの出力用構造体に変換
PSGBufferOut EncodeGBuffer(in GBufferData data, matrix view_projection_matrix)
{
    PSGBufferOut ret = (PSGBufferOut) 0;
    //  ベースカラーはbase_color(RT0)に出力
    ret.baseColor = data.baseColor;
    //  自己発光色はemissive_color(RT1)に出力
    ret.emissiveColor.rgb = data.emissiveColor;
    ret.emissiveColor.a = 1;
    //  法線はnormal(RT2)に出力
    ret.normal.rgb = data.wNormal;
    ret.normal.a = 1;
    //  粗さ/金属質/遮蔽はparameter(RT3)に出力
    ret.parameter.r = data.occlusionFactor;
    ret.parameter.g = data.roughness;
    ret.parameter.b = data.metalness;
    ret.parameter.a = data.occlusionStrength;
    //  深度はdepth(RT4)に出力
    float4 position = mul(float4(data.wPosition, 1.0f), view_projection_matrix);
    ret.depth = position.z / position.w;
    return ret;
};
