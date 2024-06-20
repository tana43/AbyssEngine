#pragma once
#include "Component.h"
#include "StaticMesh.h"
#include "CollisionMesh.h"

class Stage : public AbyssEngine::Component
{
public:
    Stage() {}
    ~Stage() { collisionModels_.clear(); }

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
private:

	struct GltfCollisionMesh
	{
		struct Triangle
		{
			DirectX::XMFLOAT3	positions[3];
			DirectX::XMFLOAT3	normal;
		};
		struct Area
		{
			DirectX::BoundingBox	boundingBox;
			std::vector<int>		triangleIndices;
		};

		std::vector<Triangle>	triangles;
		std::vector<Area>		areas;
	};

    //�X�e�[�W�ɔz�u����A�N�^�[��ǉ�
    void AddStageModel(const std::string& actorName,const std::string& modelPath);

	//�o�^����Ă��邷�ׂẴ��f�����_����O�p�`��o�^���Ă���
	void RegisterTriangles();

    //�z�u���郂�f����o�^���Ă������߂̃x�N�^�[�ϐ�
    std::vector<std::shared_ptr<AbyssEngine::GltfCollisionMesh>> collisionModels_;
};

