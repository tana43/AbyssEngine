#pragma once
#include "Renderer.h"
#include "ComputeParticleSystem.h"

namespace AbyssEngine
{
    class Texture;

    //シェーダーの授業ベースGPUパーティクル
    class ComputeParticleEmitter : public Renderer
    {
    public:
        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Render()override;
        void RecalculateFrame()override;
        void DrawImGui()override;

        void Update();

        //パーティクル生成
        void EmitParticle();

    private:
        //std::shared_ptr<ComputeParticleSystem> particleSystem_;
        std::shared_ptr<Texture> texture_;

        //　一度に生成されるパーティクルの数
        int emitNum_ = 100;

        // パーティクル寿命
        float lifespan_ = 1.0f;

        //テクスチャのタイプ 
        int texType_ = 0;

        // 生成範囲の指定
        //Vector3 emitAreaMin_ = { 0,0,0 };
        //Vector3 emitAreaMax_ = { 1,1,1 };

        //　生成位置の振れ幅
        Vector3 positionAmplitude_ = {};
        //　初期速度
        Vector3 velocity_ = {0.0f,10.0f,0.0f};
        //　生成速度の振れ幅
        Vector3 velocityAmplitude_ = {3.0f,1.0f,3.0f};
        //　初期加速度
        Vector3 acceleration_ = {0.0f,-3.0f,0.0f};
        //　生成加速度の振れ幅
        Vector3 accelerationAmplitud_ = {};

        //　生成スケールの振れ幅
        Vector2 scaleAmplitude_ = {};
        //　初期スケール速度
        Vector2 scaleVelocity_ = {};
        //　生成スケール速度の振れ幅
        Vector2 scaleVelocityAmplitude_ = {};
        //　初期スケール加速度
        Vector2 scaleAcceleration_ = {};
        //　生成スケール加速度の振れ幅
        Vector2 scaleAccelerationAmplitud_ = {};

        //　生成回転の振れ幅
        Vector3 rotationAmplitude_ = {};
        //　初期回転速度
        Vector3 rotationVelocity_ = {};
        //　生成回転速度の振れ幅
        Vector3 rotationVelocityAmplitude_ = {};
        //　初期回転加速度
        Vector3 rotationAcceleration_ = {};
        //　生成回転加速度の振れ幅
        Vector3 rotationAccelerationAmplitud_ = {};

        float brightness_ = 1.0f;//colorの乗数

        Vector4 color_ = {1,1,1,1};

        //　色の振れ幅
        Vector4 colorAmplitud_ = {1,1,1,1};

        //Xキーを押すことで生成可能にする
#if _DEBUG
        bool canKeyXEmit_ = true;
#else
        bool canKeyXEmit_ = false;
#endif // _DEBUG

    };
}


