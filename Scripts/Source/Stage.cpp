#include "Stage.h"
#include "Actor.h"
#include "Engine.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include "PrimitiveRenderer.h";
#include "DebugRenderer.h";
#include "LineRenderer.h";

#include "CollisionHelper.h"

#include <DirectXCollision.h>
#include "imgui/imgui.h"

using namespace AbyssEngine;

void Stage::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();
}

void Stage::DrawDebug()
{
#if _DEBUG
	const auto& priRenderer = Engine::renderManager_->primitiveRenderer_;
	int i = 0;
	for (const auto& triangle : triangles_)
	{
		if (i >= showTriangleCount_)break;

		const Vector4 edgeColor = { 1,1,1,1 };

		priRenderer->AddVertex(triangle.positions[0], edgeColor);
		priRenderer->AddVertex(triangle.positions[1], edgeColor);
		priRenderer->AddVertex(triangle.positions[1], edgeColor);
		priRenderer->AddVertex(triangle.positions[2], edgeColor);
		priRenderer->AddVertex(triangle.positions[2], edgeColor);
		priRenderer->AddVertex(triangle.positions[0], edgeColor);
		i++;
	}
#endif // _DEBUG
}

bool Stage::DrawImGui()
{
#if _DEBUG
	if (ImGui::TreeNode("Stage"))
	{
		ImGui::DragInt("Show Triangles", &showTriangleCount_);

		ImGui::TreePop();
	}
#endif // _DEBUG
	return true;
}

bool Stage::RayCast(const Vector3& start, const Vector3& end, Vector3& hitPosition, Vector3& hitNormal)
{
	//���C�̒������O�̂Ƃ��͏������Ȃ�
	if (Vector3(start - end).LengthSquared() == 0)return false;
	
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
	//��ԕ��������f�[�^���g���A���C�L���X�g���S���ɏ�������
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

bool Stage::SphereCast(const AbyssEngine::Vector3& origin, const AbyssEngine::Vector3& direction, float radius, float& distance, AbyssEngine::Vector3& hitPosition, AbyssEngine::Vector3& hitNormal,bool drawDebug)
{
	bool hit = false;

	for (const Collider::Triangle& triangle : triangles_)
	{
		DirectX::XMVECTOR Positions[3] = {
			DirectX::XMLoadFloat3(&triangle.positions[0]),
			DirectX::XMLoadFloat3(&triangle.positions[1]),
			DirectX::XMLoadFloat3(&triangle.positions[2])
		};

		Collision::HitResult result;
		if(Collision::IntersectSphereCastVsTriangle(
			origin,direction,distance,radius,Positions,&result
		))
		{
			if (distance < result.distance) continue;
			distance = result.distance;
			hitPosition = result.position;
			hitNormal = result.normal;
			hit = true;
		}
	}

	//�f�o�b�O�\��
	if (drawDebug)
	{
		auto& debugR = Engine::renderManager_->debugRenderer_;
		auto& lineR = Engine::renderManager_->lineRenderer_;
		const Vector3 pos = origin + direction * distance;
		Vector4 color;
		if (hit)color = { 1,0,0,0.5f };
		else color = { 1,1,0,0.5f };
		debugR->DrawSphere(origin, radius, color);
		debugR->DrawSphere(pos, radius, color);
		lineR->AddVertex(origin, color);
		lineR->AddVertex(pos, color);
	}

	return hit;
}

std::shared_ptr<AbyssEngine::Actor> Stage::AddStageModel(const std::string& actorName,std::string modelPath)
{
	//�A�N�^�[�𐶐�
    const auto& a = Engine::sceneManager_->GetActiveScene().InstanceActor(actorName);

	//���f���ǂݍ���
    a->AddComponent<StaticMesh>(modelPath.c_str());

	//����p���f���ǂݍ���
	int ext = modelPath.find_last_of(".");//�g���q�̎n�߂��������ڂ����擾
    modelPath.insert(ext, "_Collision");
    const auto& mesh = a->AddComponent<MeshCollider>(modelPath.c_str());

	//�X�e�[�W�Ɛe�q�t��
    a->SetParent(actor_);

    meshColliders_.emplace_back(mesh);

	return a;
}

void Stage::RegisterTriangles()
{
    //��x�o�^���ꂽ�O�p�`���N���A
    triangles_.clear();

    //�ēo�^
    for (const auto& model : meshColliders_)
    {
        for (const auto& tri : model->triangles_)
        {
            triangles_.emplace_back(tri);
        }
    }
}

void Stage::SpaceDivision()
{

}
