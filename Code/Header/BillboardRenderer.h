#pragma once
#include "Renderer.h"
#include <d3d11.h>
#include <wrl.h>

#include "ConstantBuffer.h"

namespace AbyssEngine
{
    class Texture;

    //テクスチャ付きの板ポリ
    class BillboardRenderer : public Renderer
    {
    public:
        void Initialize(const std::shared_ptr<Actor>& actor)override;

        //行列などの計算
        void RecalculateFrame()override;

        //描画
        void Render()override;

        bool DrawImGui()override;

    private:
        void SetActive(bool value)override;//アクティブ状態を切り替える
    public:
        struct Constants
        {
            Matrix worldMatrix_;
            Vector4 color_ = {1,1,1,1};
        };

    public:
        void SetVisibility(const bool& flag) { visibility_ = flag; }

        const std::unique_ptr<ConstantBuffer<Constants>>& GetConstantBuffer() { return constantBuffer_; }

    private:
        // 頂点構造体
        struct Vertex 
        {
            Vector3 position_;
            Vector2 texcoord_;
        };

    private:
        Vector2 pivot_;//中心点
        Vector2 cropSize_;//テクスチャの切り取りサイズ

        std::shared_ptr<Texture> texture_; //テクスチャ

        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer_;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;

        bool billboard_ = true;//ビルボードオン・オフ

        bool visibility_ = true;//表示・非表示

        //定数バッファ
        std::unique_ptr<ConstantBuffer<Constants>> constantBuffer_;
    };
}