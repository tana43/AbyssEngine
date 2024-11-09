#pragma once
#include "Renderer.h"
#include "ConstantBuffer.h"

namespace AbyssEngine
{
    class Texture;

    class TrailRenderer : public Renderer
    {
    public:
        TrailRenderer() {}
        ~TrailRenderer() {}

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Update();
        void Render()override;
        void RecalculateFrame()override;
        void DrawImGui()override;

    public:
        void SetWidth(float width) { width_ = width; }
        void SetColor(Vector4 col) { color_ = col; }
        void SetIntensity(float intensity) { constantBuffer_->data_.intensity_ = intensity; }

    public:
        struct Constant
        {
            DirectX::XMFLOAT4 color_ = { 1,1,1,1 };
            //DirectX::XMFLOAT2 uvScrollSpeed_;
            float intensity_ = 1.0f;
        };
        const std::unique_ptr<ConstantBuffer<Constant>>& GetConstantBuffer() { return constantBuffer_; }

    private:
        static constexpr UINT VertexCapacity = 1024;
        static constexpr int MAX_POLYGON = 32;

        struct Vertex
        {
            DirectX::XMFLOAT3	position_;
            DirectX::XMFLOAT4	color_;
            DirectX::XMFLOAT2   texcoord_ = {1,1};
        };
        std::vector<Vertex>		vertices_;

        //初期位置の初期化フラグ
        bool isInitPosition_ = false;

        //生成されるトレイルの幅
        float width_ = 0.1f;

        //頂点を登録する更新間隔
        float updateFrequency_ = 0.03f;

        Vector4 color_ = {1,1,1,1};

        //トレイルする座標配列
        struct TrailData
        {
            Vector3 position_;
            Vector3 moveDirection_; // 進行方向
        };
        TrailData trailDatas[MAX_POLYGON];

        //オフセット座標
        Vector3 offsetPosition_ = {};

        //前回の座標
        Vector3 previousPosition_;


        Microsoft::WRL::ComPtr<ID3D11VertexShader>	     vertexShader_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>	     pixelShader_;
        Microsoft::WRL::ComPtr<ID3D11InputLayout>	     inputLayout_;
        Microsoft::WRL::ComPtr<ID3D11Buffer>		     vertexBuffer_;
        std::unique_ptr<ConstantBuffer<Constant>>        constantBuffer_;
        std::shared_ptr<Texture> texture_;

    
    private:
        void AddVertex(const Vector3& position, const Vector2& uv,const Vector4& color);

        //トレイルデータからポリゴンを作成する
        void CreatePolygon(const TrailData& trailData,const float& texcoordX,const float& width);

    };
}

