#pragma once
#include "ScriptComponent.h"
#include "StaticMesh.h"
#include "MeshCollider.h"

//�X�e�[�W���Ǘ�������N���X
//��ԕ����ɑΉ��������C�L���X�g�p
namespace AbyssEngine
{
    class Stage : public AbyssEngine::ScriptComponent
    {
    public:
        Stage() {}
        ~Stage() { meshColliders_.clear(); }

        void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);

        //void Update()override;
        void DrawDebug()override;
        void DrawImGui()override;

        struct Area
        {
            DirectX::BoundingBox	boundingBox;
            std::vector<int>		triangleIndices;
        };
        std::vector<Area> areas;

        bool RayCast(
            const AbyssEngine::Vector3& start,
            const AbyssEngine::Vector3& end,
            AbyssEngine::Vector3& hitPosition,
            AbyssEngine::Vector3& hitNormal);

        bool SphereCast(
            const AbyssEngine::Vector3& origin,
            const AbyssEngine::Vector3& direction,
            float radius,
            float& distance,
            AbyssEngine::Vector3& hitPosition,
            AbyssEngine::Vector3& hitNormal,
            bool drawDebug = false
        );

        //�X�e�[�W�ɔz�u����A�N�^�[��ǉ�
        std::shared_ptr<AbyssEngine::Actor> AddStageModel(const std::string& actorName, std::string modelPath);

        //�o�^����Ă��邷�ׂẴ��f�����_����O�p�`��o�^���Ă���
        void RegisterTriangles();
    protected:

        //��ԕ���
        void SpaceDivision();

        //�z�u���郂�f����o�^���Ă������߂̃x�N�^�[�ϐ�
        std::vector<std::weak_ptr<AbyssEngine::MeshCollider>> meshColliders_;

        //�e���f���̎O�p�`���܂Ƃ߂ĊǗ�
        std::vector <AbyssEngine::Collider::Triangle > triangles_;

        //��ԕ����I���A�I�t
        bool spaceDivisionEnabled_ = false;

        //���݃��C����΂��ꂽ�G���A
        int currentArea_;

#if _DEBUG
        int showTriangleCount_ = 2000;
#endif // _DEBUG

        friend class StageManager;
    };
}

