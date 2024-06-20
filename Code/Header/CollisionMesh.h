#pragma once
#include "Component.h"
#include "GltfCollisionMesh.h"


namespace AbyssEngine
{
    //当たり判定用モデルクラス　レンダラーを継承させた方が都合がいいのでそうしている
    class CollisionMesh : public Renderer
    {
    public:
        void Initialize(const std::shared_ptr<Actor>& actor)override;

        const std::shared_ptr<GltfCollisionMesh>& GetMesh() { return mesh_; }

    private:
        std::shared_ptr<GltfCollisionMesh> mesh_;
    };
}

