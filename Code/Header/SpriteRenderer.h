#pragma once
#include <DirectXMath.h>
#include <memory>
#include <wrl.h>
#include <d3d11.h>
#include <string>

namespace AbyssEngine
{
    class Texture;
    class Material;

    //スプライトレンダラーコンポーネント
    class SpriteRenderer : public 
    {
    private:
        struct Vertex
        {
            DirectX::XMFLOAT3 pos_;     //位置
            DirectX::XMFLOAT2 tex_;     //UV位置
            DirectX::XMFLOAT4 color_;   //頂点色
        };

    private:
        //void Initialize(const std::shared_ptr<GameObject>& obj)override;

    public:
        //仮実装
        void Initialize();
        void Render();      //描画実行
        void RecalculateFrame();//表示座標を計算する

    private:
        DirectX::XMFLOAT2 size_ = { 100,100 };   //表示サイズ
        DirectX::XMFLOAT2 uvOrigin_ = { 0,0 };   //テクスチャのUV原点
        DirectX::XMFLOAT2 uvSize_ = { 100,100 }; //テクスチャから切り出すサイズ（UV原点基準）
        DirectX::XMFLOAT4 color_ = { 1,1,1,1 };  //テクスチャカラー
        std::shared_ptr<Texture> texture_;       //描画するテクスチャ

        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;

        std::string fileName_;
        std::string filePath_;
        
        bool visibility_ = false;//表示切り替え
    };
}


