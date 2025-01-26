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
        struct EmitParameter
        {
            //　一度に生成されるパーティクルの数
            int emitNum_ = 100;

            // 生存時間
            float lifespan_ = 1.0f;
            //　生存時間振れ幅
            float lifespanAmplitude_ = 0.0f;

            //　生成時間
            float emitTime_ = 1.0f;

            //  生成開始時間
            //float emitStartTime_ = 0.0f;

            //テクスチャのタイプ 
            int texType_ = 0;

            // 生成範囲の指定
            //Vector3 emitAreaMin_ = { 0,0,0 };
            //Vector3 emitAreaMax_ = { 1,1,1 };

            //　生成位置の振れ幅
            Vector3 positionAmplitude_ = {};
            //　初期速度
            Vector3 velocity_ = { 0.0f,10.0f,0.0f };
            //　生成速度の振れ幅
            Vector3 velocityAmplitude_ = { 3.0f,1.0f,3.0f };
            //　初期加速度
            Vector3 acceleration_ = { 0.0f,-3.0f,0.0f };
            //　生成加速度の振れ幅
            Vector3 accelerationAmplitud_ = {};

            // 初期スケール
            Vector3 scaleInit_ = {1.0f,1.0f,1.0f};

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

            float intensity_ = 1.0f;//colorの乗数

            Vector4 color_ = { 1,1,1,1 };

            //　色の振れ幅
            Vector4 colorAmplitud_ = { 0,0,0,0 };

            void DrawImGui();
        };

    public:
        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Render()override;
        void RecalculateFrame()override;
        void DrawImGui()override;
        void DrawDebug()override;

        void Update();

        //同じコンポーネントを複数アタッチ可能か
        bool CanMultiple() override { return true; }

        //パーティクル生成
        void EmitParticle(const EmitParameter& param);
        void EmitParticle();

        //アセット化したパラメーターをセットする
        void SetEmitParamater(std::string filename);
        static EmitParameter GetJsonEmitParamater(std::string filename);
        void SetEmitParamater(const EmitParameter& param) { mainParam_ = param; };

        void SetUseTransform(const bool& flag) { useTransform_ = flag; }
        void SetEmitPositionNotUseTransform(const Vector3& pos) { emitPositionNotUseTransform_ = pos; }

        EmitParameter GetEmitParamter() { return mainParam_; }
    private:
        //std::shared_ptr<ComputeParticleSystem> particleSystem_;
        //std::shared_ptr<Texture> texture_;

        EmitParameter mainParam_;
        

        //Xキーを押すことで生成可能にする
#if _DEBUG
        bool canInputEmit_ = false;
        bool enableTimeline_ = true;
        EmitParameter debugParam_;
#else
        bool canInputEmit_ = false;
        bool enableTimeline_ = false;
#endif // _DEBUG
        bool imguiButton_;

        //トランスフォームコンポーネントに位置を依存するか
        bool useTransform_ = true;
        //トランスフォームに依存しない場合に参照される生成位置
        Vector3 emitPositionNotUseTransform_ = {0,0,0};
        
    private:
        void AssetCreation(const EmitParameter& param,const std::string& filename);
    };
}


