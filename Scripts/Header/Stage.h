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
private:

    //�X�e�[�W�ɔz�u����A�N�^�[��ǉ�
    void AddStageModel(const std::string& actorName,const std::string& modelPath);

	//�o�^����Ă��邷�ׂẴ��f�����_����O�p�`��o�^���Ă���
	void RegisterTriangles();

    //�z�u���郂�f����o�^���Ă������߂̃x�N�^�[�ϐ�
    std::vector<std::shared_ptr<AbyssEngine::MeshCollider>> meshColliders_;

    //�e���f���̎O�p�`���܂Ƃ߂ĊǗ�
    std::vector <AbyssEngine:: Collider::Triangle > triangles_;
};

