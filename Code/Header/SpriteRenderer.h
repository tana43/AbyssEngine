#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "Renderer.h"

namespace AbyssEngine
{
    class Texture;
    class Material;

    //スプライトレンダラーコンポーネント
    class SpriteRenderer : public Renderer
    {
    public:
        SpriteRenderer() {}
        ~SpriteRenderer() {}

    private:
        struct Vertex
        {
            Vector3 pos_;     //位置
            Vector2 tex_;     //UV位置
            Vector4 color_;   //頂点色
        };

    private:
        //void Initialize(const std::shared_ptr<GameObject>& obj)override;

    public:
        //仮実装
        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Render()override;      //描画実行
        void RecalculateFrame();//表示座標を計算する

        bool DrawImGui()override;

    private:
        void SetActive(const bool value)override;//アクティブ状態を設定する
    private:
        Vector2 size_ = { 100,100 };   //表示サイズ
        Vector2 uvOrigin_ = { 0,0 };   //テクスチャのUV原点
        Vector2 uvSize_ = { 100,100 }; //テクスチャから切り出すサイズ（UV原点基準）
        Vector4 color_ = { 1,1,1,1 };  //テクスチャカラー
        std::shared_ptr<Texture> texture_;       //描画するテクスチャ

        Microsoft::WRL::ComPtr<ID3D11Buffer>        vertexBuffer_;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>   pixelShader_;
        Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout_;
        
        bool visibility_ = true;//表示切り替え(trueなら表示、falseなら非表示)
    };
}


