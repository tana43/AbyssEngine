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

    //ステージに配置するアクターを追加
    void AddStageModel(const std::string& actorName,const std::string& modelPath);

	//登録されているすべてのモデル頂点から三角形を登録していく
	void RegisterTriangles();

    //配置するモデルを登録しておくためのベクター変数
    std::vector<std::shared_ptr<AbyssEngine::GltfCollisionMesh>> collisionModels_;
};

