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

        void DrawImGui()override;

    private:
        void SetActive(bool value)override;//アクティブ状態を切り替える
    public:
        struct Constants
        {
            Matrix worldMatrix_;
            Vector4 color_ = {1,1,1,1};
        };

    public:
        const bool& GetVisibilty() const { return visibility_; }
        void SetVisibility(const bool& flag) { visibility_ = flag; }

        const std::unique_ptr<ConstantBuffer<Constants>>& GetConstantBuffer() { return constantBuffer_; }

        void SetOffsetPos(const Vector3& pos) { offsetPos_ = pos; }
        void SetRotationZ(const float& rot) { rotZ_ = rot; }
        void SetScale(const float& sca) { offsetScale_ = sca; }

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
        Vector3 offsetPos_;//オフセット座標

        float rotZ_ = 0.0f;//オフセットｚ回転

        float offsetScale_ = 1.0f;//オフセットスケール

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