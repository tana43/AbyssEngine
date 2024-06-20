#pragma once
#include "Component.h"
#include "GltfCollisionMesh.h"


namespace AbyssEngine
{
    //�����蔻��p���f���N���X�@�����_���[���p�������������s���������̂ł������Ă���
    class CollisionMesh : public Renderer
    {
    public:
        void Initialize(const std::shared_ptr<Actor>& actor)override;

        const std::shared_ptr<GltfCollisionMesh>& GetMesh() { return mesh_; }

    private:
        std::shared_ptr<GltfCollisionMesh> mesh_;
    };
}

