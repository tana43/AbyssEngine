#include "Constants.hlsli"

//パーティクルスレッド数
static const int Num_Particle_Thread = 1024;

//生成パーティクル構造体
struct EmitParticleData
{
    float4 parameter; //x : パーティクル処理タイプ,y : 生成時間,zw : 空き
    
    float4 position;
    float4 rotation;
    float4 scale;
    
    float4 velocity;
    float4 acceleration;
    
    float4 color;
};

//パーティクル構造体
struct ParticleData
{
    float4 parameter;//x : パーティクル処理タイプ,y : 生存時間.z : 生存フラグ, w : 空き
    float4 position;
    float4 rotation;
    float4 scale;
    
    float4 velocity;
    float4 acceleration;
    
    float4 texcoord;
    float4 color;
};

//indiredctDataBufferへのアクセス用バイトオフセット
static const uint Indirect_Arguments_Num_Current_Particle = 0;
static const uint Indirect_Arguments_Num_Pravious_Particle = 4;
static const uint Indirect_Arguments_Num_Dead_Particle = 8;
static const uint Indirect_Arguments_Emit_Particle_Dispatch_Indirect = 12;

//汎用情報
cbuffer COMPUTE_PARTICLE_COMMON_CONSTANT_BUFFER : register(b10)
{
    float deltaTime;
    uint2 textureSplitCount;
    uint systemNumParticles;
    
    uint totalEmitCount; //　生成予定のパーティクル数
    uint commonDummy[3];
}

//頂点シェーダーからジオメトリシェーダーに転送する情報
struct GS_IN
{
    uint vertexId : VERTEX_ID;
};

//ジオメトリシェーダーからピクセルシェーダーに転送する情報
struct PS_IN
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float2 texcoord : TEXCOORD;
};