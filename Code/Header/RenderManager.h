#pragma once

#include <vector>
#include "MathHelper.h"

namespace AbyssEngine
{
    class Renderer;
    class SpriteRender;

    class RenderManager
    {
    public:
        RenderManager();

        void Reset();
        void Add(const std::shared_ptr<SpriteRender>& mRend);//マネージャーにレンダラー登録する

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

