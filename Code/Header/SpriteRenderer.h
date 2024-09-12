#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "Renderer.h"

namespace AbyssEngine
{
    class Texture;

    //スプライトレンダラーコンポーネント
    class SpriteRenderer : public Renderer
    {
    public:
        SpriteRenderer() {}
        ~SpriteRenderer() {}

        //引数alphaの値へフェードイン・アウトする フェード完了時にTrueを返す
        bool FadeIn(float alpha, float changeSpeed);
        bool FadeOut(float alpha, float changeSpeed);

    private:
        struct Vertex
        {
            Vector3 pos_;     //位置
            Vector2 tex_;     //UV位置
            Vector4 color_;   //頂点色
        };

    public:
        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Render()override;      //描画実行
        void RecalculateFrame();//表示座標を計算する

        void DrawImGui()override;

    public:
        void SetColor(const Vector4& color) { color_ = color; }
        const Vector4& GetColor() const { return color_; }

        void SetColorAlpha(const float alpha) { color_.w = alpha; }
        const float& GetColorAlpha() const { return color_.w; }

        void SetSize(const Vector2& size) { size_ = size; }
        const Vector2& GetSize() const { return size_; }

        void SetOffsetPosition(const Vector2& pos) { offsetPos_ = pos; }
        const Vector2& GetOffsetPosition() const { return offsetPos_; }

        void SetPivot(const Vector2& pivot) { pivot_ = pivot; }
        const Vector2& GetPivot() const { return pivot_; }

        void SetScale(const Vector2& sca) { scale_ = sca; }
        const Vector2& GetScale() const { return scale_; }

        void SetAngle(const float& angle) { angle_ = angle; }
        const float& GetAngle() const { return angle_; }

    private:
        void SetActive(const bool value)override;//アクティブ状態を設定する
    private:
        Vector2 size_ = { 100,100 };   //表示サイズ
        Vector2 scale_ = { 1,1 };   //スケール
        Vector2 uvOrigin_ = { 0,0 };   //テクスチャのUV原点
        Vector2 uvSize_ = { 100,100 }; //テクスチャから切り出すサイズ（UV原点基準）
        Vector4 color_ = { 1,1,1,1 };  //テクスチャカラー
        Vector2 pivot_ = { 0,0 };//中心点
        Vector2 offsetPos_ = { 0,0 };//オフセット座標
        float angle_ = 0.0f; //角度
        std::shared_ptr<Texture> texture_;       //描画するテクスチャ

        Microsoft::WRL::ComPtr<ID3D11Buffer>        vertexBuffer_;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>   pixelShader_;
        Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout_;
        
        bool visibility_ = true;//表示切り替え(trueなら表示、falseなら非表示)
    };
}


