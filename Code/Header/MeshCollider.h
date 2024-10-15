#pragma once
#include "GltfCollisionMesh.h"
#include "Collider.h"

namespace AbyssEngine
{
    //当たり判定用モデルクラス　レンダラーを継承させた方が都合がいいのでそうしている
    class MeshCollider : public AbyssEngine::Collider
    {
    public:
        MeshCollider() {}
        ~MeshCollider() {}

        void Initialize(const std::shared_ptr<Actor>& actor)override;

        const std::shared_ptr<GltfCollisionMesh>& GetModel() { return model_; }

        //行列変換　※現在の位置を基準に変換するので複数回この関数を呼び出すと、そこからさらに行列分ずれることになる
        void Transform();//自分のTransformを使って変換
        void Transform(const DirectX::XMFLOAT4X4& worldTransform);

        void DrawImGui()override;
        void DrawDebug()override;

    private:
        //ポリゴンを抽出
        void RegisterTriangles();

    public:
        std::vector<Collider::Triangle>	triangles_;

    private:
        std::shared_ptr<GltfCollisionMesh> model_;
    };
}

