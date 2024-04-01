#pragma once

#include <vector>
#include "MathHelper.h"
#include "Renderer.h"

namespace AbyssEngine
{
    class SpriteRenderer;

    class RenderManager
    {
    public:
        RenderManager();

        void Reset();
        void Add(const std::shared_ptr<SpriteRenderer>& mRend);//マネージャーにレンダラー登録する

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
        //std::vector<std::weak_ptr<Renderer>> renderer3DList_{};
        std::vector<std::weak_ptr<Renderer>> renderer2DList_{};

        //void RenderMain();

        //2Dオブジェクトのレンダリング
        void Render2D() const;

        //Rendererの生存確認
        void CheckRenderer();
    };

}

