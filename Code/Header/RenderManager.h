#pragma once

#include <vector>
#include "MathHelper.h"
#include "Renderer.h"

namespace AbyssEngine
{
    class SpriteRenderer;
    class SkeltalMesh;
    class Camera;

    class RenderManager
    {
    public:
        RenderManager();

        void Reset();
        void Add(const std::shared_ptr<SpriteRenderer>& mRend);//マネージャーにレンダラーを登録する
        void Add(const std::shared_ptr<SkeltalMesh>& mRend);//マネージャーにレンダラーを登録する
        void Add(const std::shared_ptr<Camera>& camera);//マネージャーにカメラを登録する

        void Render(); //描画実行

        //bool renderGame_ = true; //ゲームビューを描画する
        //bool renderScene_ = true; //シーンビューを描画する

    private:
        struct Vertex
        {
            Vector3 pos_;
            Vector2 tex_;
            Vector4 color_;
        };

        Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_;


    private:
        std::vector<std::weak_ptr<Renderer>> renderer3DList_{};
        std::vector<std::weak_ptr<Renderer>> renderer2DList_{};
        
        std::vector<std::weak_ptr<Camera>> cameraList_{};

        //今は最低限必要なものだけ用意
        struct ConstantBufferScene
        {
            Matrix viewProjectionMatrix_;
            //Matrix shadowMatrix_;
            //Vector4 cameraDirection_;
            Vector4 cameraPosition_;
            Vector4 lightDirection_;
            Vector3 lightColor_;
            float pad;
        };

        ConstantBufferScene bufferScene_;
        Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferScene_;

        //2Dオブジェクトのレンダリング
        void Render2D() const;

        void Render3D(const std::shared_ptr<Camera>& camera);

        //Rendererの生存確認
        void CheckRenderer();

        void UpdateConstantBuffer()const;
    };

}

