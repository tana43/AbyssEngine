#pragma once

#include <vector>
#include "MathHelper.h"
#include "Renderer.h"

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

        void Reset();
        void Add(const std::shared_ptr<SpriteRenderer>& mRend);//マネージャーにレンダラーを登録する
        void Add(const std::shared_ptr<SkeletalMesh>& mRend);//マネージャーにレンダラーを登録する
        //void Add(const std::shared_ptr<GltfSkeletalMesh>& mRend);//マネージャーにレンダラーを登録する
        void Add(const std::shared_ptr<StaticMesh>& mRend);//マネージャーにレンダラーを登録する
        void Add(const std::shared_ptr<Camera>& camera_);//マネージャーにカメラを登録する

        void Render(); //描画実行

        void DrawImGui();

        //bool renderGame_ = true; //ゲームビューを描画する
        //bool renderScene_ = true; //シーンビューを描画する

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
            Matrix inverseViewProjection_;
            //Matrix shadowMatrix_;
            //Vector4 cameraDirection_;
            Vector4 lightDirection_ = {0,0,1,0};
            Vector4 eyePosition_;
            Vector4 focusPosition_;
            //Vector3 lightColor_;
            float exposure_ = 1.0f;
            float pureWhite_ = 3.0f;
            float emissiveIntensity_ = 50.0f;
            float imageBasedLightingIntensity_ = 1.0f;
            float skyboxRoughness_ = 1.0f;
            float time_ = 0.0f;
        };

        ConstantBufferScene bufferScene_;
        Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferScene_;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> iblShaderResourceView_[4];

        //オフスクリーンレンダリング
        std::unique_ptr<FrameBuffer> frameBuffer_;
        std::unique_ptr<FullscreenQuad> bitBlockTransfer_;
        std::unique_ptr<Bloom> bloom_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;

    private:

        //2Dオブジェクトのレンダリング
        void Render2D() const;

        void Render3D(const std::shared_ptr<Camera>& camera_);

        //Rendererの生存確認
        void CheckRenderer();

        void UpdateConstantBuffer()const;

        void IBLInitialize();
        void IBLSetResources();
    };

}

