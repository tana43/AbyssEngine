#pragma once
#include "ConstantBuffer.h"
#include "Renderer.h"

namespace AbyssEngine
{
    class Texture;
    class StaticMesh;

    class SwordTrailRenderer : public Renderer
    {
    public:
        SwordTrailRenderer() {}
        ~SwordTrailRenderer() {}

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Update();

        void Render()override;
        void RecalculateFrame()override;
        void DrawImGui()override;

    public:
        void SetColor(Vector4 col) { color_ = col; }
        void SetIntensity(float intensity) { constantBuffer_->data_.intensity_ = intensity; }
        void SetSwordRange(const float& range) { swordRange_ = range; }

        void SetAttachedMesh(const std::shared_ptr<StaticMesh>& t) { attachedMesh_ = t; }
        void SetOffsetPosition(const Vector3& pos) { offsetPosition_ = pos; }
        void SetOffsetRotation(const Vector3& rot) { offsetRotation_ = rot; }

        void SetActiveTrail(bool active) { activeTrail_ = active; }

    public:
        struct Constant
        {
            DirectX::XMFLOAT4 color_ = { 1,1,1,1 };
            //DirectX::XMFLOAT2 uvScrollSpeed_;
            float intensity_ = 1.0f;
        };
        const std::unique_ptr<ConstantBuffer<Constant>>& GetConstantBuffer() { return constantBuffer_; }

    private:
        void AddVertex(AbyssEngine::Vector3 pos, AbyssEngine::Vector2 uv,AbyssEngine::Vector4 color);

        static constexpr UINT VertexCapacity = 1024;
        static constexpr int MAX_POLYGON = 8;

        struct Vertex
        {
            DirectX::XMFLOAT3	position_;
            DirectX::XMFLOAT4   color_ = {1,1,1,1};
            DirectX::XMFLOAT2   texcoord_ = { 1,1 };
        };
        std::vector<Vertex>		vertices_;

        //対数で保存するために変数をバッファと別に持っておく
        float intensity_ = 1.0f;

        //頂点を登録する更新間隔
        //float updateFrequency_ = 0.03f;

        Vector4 color_ = { 1,1,1,1 };

        //トレイルする座標配列
        struct TrailData
        {
            Vector3 head_;//剣の先端の座標
            Vector3 tail_;//剣の末端の座標
            Vector4 color_ = {1,1,1,1};//カラー
        };
        TrailData trailDatas[MAX_POLYGON];

        //オフセット座標
        Vector3 offsetPosition_ = {};
        Vector3 offsetRotation_ = {};

        Microsoft::WRL::ComPtr<ID3D11VertexShader>	     vertexShader_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>	     pixelShader_;
        Microsoft::WRL::ComPtr<ID3D11InputLayout>	     inputLayout_;
        Microsoft::WRL::ComPtr<ID3D11Buffer>		     vertexBuffer_;
        std::unique_ptr<ConstantBuffer<Constant>>        constantBuffer_;
        std::shared_ptr<Texture> texture_;

        float swordRange_ = 1.0f; // エフェクトの長さ

        // アタッチするワールド行列
        std::weak_ptr<StaticMesh> attachedMesh_;

        // 軌跡を出すか (falseの場合は登録される頂点のカラーを０にして非表示にする)
        bool activeTrail_ = true;
    };

}


