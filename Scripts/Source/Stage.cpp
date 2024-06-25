#include "Stage.h"
#include "Actor.h"
#include "Engine.h"
#include "SceneManager.h"

#include <DirectXCollision.h>

using namespace AbyssEngine;

void Stage::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();
}

bool Stage::RayCast(const Vector3& start, const Vector3& end, Vector3& hitPosition, Vector3& hitNormal)
{
	//レイの長さが０のときは処理しない
	if (Vector3(start - end).LengthSquared() == 0)return false;
	
	bool hit = false;
	
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(end, start);
	DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(Vec);
	float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(Vec));

	// 空間分割せず、普通にレイキャストをする
	if (!spaceDivisionEnabled_)
	{
		for (const Collider::Triangle& triangle : triangles_)
		{
			DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&triangle.positions[0]);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&triangle.positions[1]);
			DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&triangle.positions[2]);

			float dist = distance;
			if (DirectX::TriangleTests::Intersects(start, Direction, A, B, C, dist))
			{
				if (distance < dist) continue;
				distance = dist;
				hitNormal = triangle.normal;
				hit = true;
			}
		}
	}
	// TODO②：空間分割したデータを使い、レイキャストを拘束に処理する
	else
	{
		//各空間とレイとの判定
		int i = 0;
		for (auto& area : areas)
		{
			float dist = distance;
			if (area.boundingBox.Intersects(start, Direction, dist))
			{
				//当たった空間に登録されている三角形とレイキャスト
				for (const int index : area.triangleIndices)
				{
					const auto& triangle = triangles_[index];
					DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&triangle.positions[0]);
					DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&triangle.positions[1]);
					DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&triangle.positions[2]);

					float dist = distance;
					if (DirectX::TriangleTests::Intersects(start, Direction, A, B, C, dist))
					{
						if (distance < dist) continue;
						distance = dist;
						hitNormal = triangle.normal;
						hit = true;
					}
				}
				currentArea_ = i;
				break;
			}
			++i;
		}
	}
	if (hit)
	{
		DirectX::XMVECTOR HitPosition = DirectX::XMVectorAdd(start, DirectX::XMVectorScale(Direction, distance));
		DirectX::XMStoreFloat3(&hitPosition, HitPosition);
	}
	return hit;
}

std::shared_ptr<AbyssEngine::Actor> Stage::AddStageModel(const std::string& actorName,std::string modelPath)
{
	//アクターを生成
    const auto& a = Engine::sceneManager_->GetActiveScene().InstanceActor(actorName);

	//モデル読み込み
    a->AddComponent<StaticMesh>(modelPath.c_str());

	//判定用モデル読み込み
	int ext = modelPath.find_last_of(".");//拡張子の始めが何文字目かを取得
    modelPath.insert(ext, "_Collision");
    const auto& mesh = a->AddComponent<MeshCollider>(modelPath.c_str());

	//ステージと親子付け
    a->SetParent(actor_);

    meshColliders_.emplace_back(mesh);

	return a;
}

void Stage::RegisterTriangles()
{
    //一度登録された三角形をクリア
    triangles_.clear();

    //再登録
    for (const auto& model : meshColliders_)
    {
        for (const auto& tri : model->triangles)
        {
            triangles_.emplace_back(tri);
        }
    }
}

void Stage::SpaceDivision()
{

}
