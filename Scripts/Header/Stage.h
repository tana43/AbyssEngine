#pragma once
#include "Component.h"
#include "StaticMesh.h"
#include "MeshCollider.h"

class Stage : public AbyssEngine::Component
{
public:
    Stage() {}
    ~Stage() { meshColliders_.clear(); }

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);

    struct Area
    {
        DirectX::BoundingBox	boundingBox;
        std::vector<int>		triangleIndices;
    };
    std::vector<Area>		areas;

    bool RayCast(
        const Vector3& start,
        const Vector3& end,
        Vector3& hitPosition,
        Vector3& hitNormal);

protected:

    //�X�e�[�W�ɔz�u����A�N�^�[��ǉ�
    void AddStageModel(const std::string& actorName,const std::string& modelPath);

	//�o�^����Ă��邷�ׂẴ��f�����_����O�p�`��o�^���Ă���
	void RegisterTriangles();

    //��ԕ���
    void SpaceDivision();

    //�z�u���郂�f����o�^���Ă������߂̃x�N�^�[�ϐ�
    std::vector<std::shared_ptr<AbyssEngine::MeshCollider>> meshColliders_;

    //�e���f���̎O�p�`���܂Ƃ߂ĊǗ�
    std::vector <AbyssEngine::Collider::Triangle > triangles_;

    //��ԕ����I���A�I�t
    bool spaceDivisionEnabled_;

    //���݃��C����΂��ꂽ�G���A
    int currentArea_;
};

