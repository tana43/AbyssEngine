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

    //ステージに配置するアクターを追加
    void AddStageModel(const std::string& actorName,const std::string& modelPath);

	//登録されているすべてのモデル頂点から三角形を登録していく
	void RegisterTriangles();

    //配置するモデルを登録しておくためのベクター変数
    std::vector<std::shared_ptr<AbyssEngine::MeshCollider>> meshColliders_;

    //各モデルの三角形をまとめて管理
    std::vector <AbyssEngine:: Collider::Triangle > triangles_;
};

