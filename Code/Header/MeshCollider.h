#pragma once
#include "GltfCollisionMesh.h"
#include "Collider.h"

namespace AbyssEngine
{
    //�����蔻��p���f���N���X�@�����_���[���p�������������s���������̂ł������Ă���
    class MeshCollider : public AbyssEngine::Collider
    {
    public:
        MeshCollider() {}
        ~MeshCollider() {}

        void Initialize(const std::shared_ptr<Actor>& actor)override;

        const std::shared_ptr<GltfCollisionMesh>& GetModel() { return model_; }

        //�s��ϊ��@�����݂̈ʒu����ɕϊ�����̂ŕ����񂱂̊֐����Ăяo���ƁA�������炳��ɍs�񕪂���邱�ƂɂȂ�
        void Transform();//������Transform���g���ĕϊ�
        void Transform(const DirectX::XMFLOAT4X4& worldTransform);

        bool DrawImGui()override;
        void DrawDebug()override;

    public:
        
        struct Triangle
        {
            DirectX::XMFLOAT3	positions[3];
            DirectX::XMFLOAT3	normal;
        };
        std::vector<Triangle>	triangles;
        

    private:
        std::shared_ptr<GltfCollisionMesh> model_;
    };
}

