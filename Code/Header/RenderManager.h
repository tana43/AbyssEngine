#pragma once

//ディファードレンダリング オン・オフ
#define ENABLE_DIFFERD_RENDERING 0

#include <vector>
#include "MathHelper.h"
#include "Renderer.h"
#include "ConstantBuffer.h"

namespace AbyssEngine
{
    class SpriteRenderer;
    class SkeletalMesh;
    class GltfSkeletalMesh;
    class StaticMesh;
    class Camera;
    class Bloom;
    class FrameBuffer;
    class FullscreenQuad;
    class Skybox;
    class CascadedShadowMap;
    class PlaneRenderer;

    class DebugRenderer;
    class LineRenderer;
    class PrimitiveRenderer;
    class ShapeRenderer;

    class ParticleSystem;

    class RenderManager
    {
    private:
        enum class SP_State {
            Point_Wrap,Linear_Wrap,Anisotropic_Wrap,
            Point_Clamp,Linear_Clamp, Anisotropic_Clamp,
            Point_Border_Opaque_Black, Linear_Border_Opaque_Black, Point_Border_Opaque_White, Linear_Border_Opaque_White,
            Comparision_Depth,Count
        };

    public:
        RenderManager();
        ~RenderManager();

        void Reset();
        void Add(const std::shared_ptr<SpriteRenderer>& mRend);//マネージャーにレンダラーを登録する
        void Add(const std::shared_ptr<PlaneRenderer>& mRend);//マネージャーにレンダラーを登録する
        void Add(const std::shared_ptr<SkeletalMesh>& mRend);//マネージャーにレンダラーを登録する
        //void Add(const std::shared_ptr<GltfSkeletalMesh>& mRend);//マネージャーにレンダラーを登録する
        void Add(const std::shared_ptr<StaticMesh>& mRend);//マネージャーにレンダラーを登録する
        void Add(const std::shared_ptr<Camera>& camera);//マネージャーにカメラを登録する

        void Render(); //描画実行

        void DrawImGui();
        
        void Beginning();//フレーム開始

        //bool renderGame_ = true; //ゲームビューを描画する
        //bool renderScene_ = true; //シーンビューを描画する

        //使用中のカメラを変更
        static void ChangeMainCamera(Camera* camera);

        //シャドウカリング用のバウンディングボックス範囲設定
        void SetShadowCullingArea(const Vector3& max,const Vector3& min);
    
    public:
        //ミューテックス取得
        std::mutex& GetMutex() { return mutex_; }

        const std::vector<std::weak_ptr<Camera>>& GetCameraList() { return cameraList_; }

    private:
        struct Vertex
        {
            Vector3 pos_;
            Vector2 tex_;
            Vector4 color_;
        };

        Microsoft::WRL::ComPtr<ID3D11SamplerState> samplers_[static_cast<int>(SP_State::Count)];


    private:
        std::vector<std::weak_ptr<Renderer>> renderer3DList_{};
        std::vector<std::weak_ptr<Renderer>> renderer2DList_{};
        
        std::vector<std::weak_ptr<Camera>> cameraList_{};

        //今は最低限必要なものだけ用意
        struct ConstantBufferScene
        {
            Matrix view_;
            Matrix projection_;
            Matrix viewProjectionMatrix_;
            Matrix inverseProjection_;
            Matrix inverseViewProjection_;
            Vector4 lightDirection_ = {-0.5f,-0.5f,-0.5f,0};
            Vector4 lightColor_ = {1,1,1,1};
            Vector4 eyePosition_;
            Vector4 focusPosition_;
            float exposure_ = 1.0f;
            float pureWhite_ = 3.0f;
            float skyboxRoughness_ = 0.0f;
            float time_ = 0.0f;
        };
        std::unique_ptr<ConstantBuffer<ConstantBufferScene>> bufferScene_;

        //フォグやシャドウなどのエフェクト定数バッファ
        //シャドウ以外は現在使っていない
        struct ConstantEffects
        {
            float fogColor_[4] = { 0.5f,0.6f,0.7f,1.0f };
            float fogDensity_ = 0.0f;
            float fogHeightFalloff_ = 0.09f;
            float groundLevel_ = -4.0f;
            float maxRayLength_ = 1000;
            float mieG_ = 0.75f; // 0.0 - 0.9

            float noiseScale_ = 0.008f;
            float timeScale_ = 0.0345f;

            float bokehAperture_ = 0.0f;
            float bokehFocus_ = 0.824f;

            float shadowDepthBias_ = 0.00002f;
            float shadowColor_ = 0.25f;
            float shadowFilterRadius_ = 4.000f;
            int shadowSampleCount_ = 16;
            int colorizeCascadedLayer_ = false;
        };
        std::unique_ptr<ConstantBuffer<ConstantEffects>> bufferEffects_;

    public:
        ConstantBuffer<ConstantBufferScene>& GetBufferScene() { return *bufferScene_.get(); }
        ConstantBuffer<ConstantEffects>& GetBufferEffects() { return *bufferEffects_.get(); }

        void SetCriticalDepthValue(const float& value) { criticalDepthValue_ = value; }

    private:
        //Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferScene_;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> iblShaderResourceView_[7];

        //オフスクリーンレンダリング
        std::unique_ptr<FrameBuffer> baseFrameBuffer_[2];//エフェクトなしのフレームバッファ
        std::unique_ptr<FrameBuffer> postEffectedFrameBuffer_;//ポストエフェクトがついたフレームバッファ
        std::unique_ptr<FullscreenQuad> bitBlockTransfer_;
        std::unique_ptr<Bloom> bloom_;

        Microsoft::WRL::ComPtr<ID3D11PixelShader> postEffectsPS_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> toneMapPS_;

        //スカイボックス
        std::unique_ptr<Skybox> skybox_;

        //カスケードシャドウマップ
        std::unique_ptr<CascadedShadowMap> cascadedShadowMap_;
        float criticalDepthValue_ = 2000.0f;
        bool enableShadow_ = true;

        //フラスタムカリング
        bool enableFrustumCulling_ = true;
        //シャドウカリング
        bool enableShadowCulling_ = true;

        //シャドウカリング用のバウンディングボックス
        DirectX::BoundingBox shadowCullingArea_;

        //G-Buffer
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> gBufferRenderTargetView_[GB_Max];
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gBufferShaderResourceView_[GB_Max];

        std::mutex mutex_;

        public:
#if _DEBUG
        //デバッグレンダラー
        std::unique_ptr<DebugRenderer>      debugRenderer_;
        std::unique_ptr<LineRenderer>       lineRenderer_;
        std::unique_ptr<PrimitiveRenderer>  primitiveRenderer_;
        std::unique_ptr<ShapeRenderer>      shapeRenderer_;
        bool enableDebugRender_ = false;
#endif // _DEBUG
        
    private:

        //2Dオブジェクトのレンダリング
        void Render2D() const;

        void Render3D(const std::shared_ptr<Camera>& camera_);
        RS_State rasterizerState3D = RS_State::Cull_None;

        //Rendererの生存確認
        void CheckRenderer();


        void UpdateConstantBuffer()const;

        void IBLInitialize();
        void IBLSetResources();

        //デバッグ限定でラスタライザーステートを数字キーで変更できる
        void DebugRSStateSelect();

        //フラスタムカリング
        //void FrustumCulling(const std::shared_ptr<Camera>& camera);

        //影描画
        void ShadowRender();

    public:
        //試し
        /*std::unique_ptr<ParticleSystem> particles_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleTexture_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise3d_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> colorTemperChart_;*/
    };

}

