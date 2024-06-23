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
	bool hit = false;
	
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(end, start);
	DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(Vec);
	float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(Vec));

	// ��ԕ��������A���ʂɃ��C�L���X�g������
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
	// TODO�A�F��ԕ��������f�[�^���g���A���C�L���X�g���S���ɏ�������
	else
	{
		//�e��Ԃƃ��C�Ƃ̔���
		int i = 0;
		for (auto& area : areas)
		{
			float dist = distance;
			if (area.boundingBox.Intersects(start, Direction, dist))
			{
				//����������Ԃɓo�^����Ă���O�p�`�ƃ��C�L���X�g
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

void Stage::AddStageModel(const std::string& actorName, const std::string& modelPath)
{
    const auto& a = Engine::sceneManager_->GetActiveScene().InstanceActor(actorName);
    a->AddComponent<StaticMesh>(modelPath.c_str());
    std::string collisionModelPath = modelPath + "_Collision";
    const auto& mesh = a->AddComponent<MeshCollider>(collisionModelPath.c_str());
    a->SetParent(actor_);

    meshColliders_.emplace_back(mesh);
}

void Stage::RegisterTriangles()
{
    //��x�o�^���ꂽ�O�p�`���N���A
    triangles_.clear();

    //�ēo�^
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
