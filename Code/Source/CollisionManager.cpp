#include "CollisionManager.h"
#include "Actor.h"

#include "SphereCollider.h"
#include "ScriptComponentManager.h"

#include "ScriptComponent.h"

#include "GameCollider.h"

#include "Engine.h"

using namespace AbyssEngine;

void CollisionManager::Initialize()
{
	meshColliderList_.clear();
	attackColliderList_.clear();
	hitColliderList_.clear();
	terrainColliderList_.clear();
}

void CollisionManager::Clear()
{
	meshColliderList_.clear();
	attackColliderList_.clear();
	hitColliderList_.clear();
	terrainColliderList_.clear();
}

void CollisionManager::Update()
{
	UpdateWorldMatrix();

	TerrainDetection();
	AttackDetection();
}

void CollisionManager::UpdateWorldMatrix()
{
#if _DEBUG
	for (const auto& collider : terrainColliderList_)
	{
		if (const auto& col = collider.lock())
		{
			col->UpdateWorldMatrix();
		}
	}

	for (const auto& collider : attackColliderList_)
	{
		if (const auto& col = collider.lock())
		{
			col->UpdateWorldMatrix();
		}
	}

	for (const auto& collider : hitColliderList_)
	{
		if (const auto& col = collider.lock())
		{
			col->UpdateWorldMatrix();
		}
	}
#else
	for (const auto& collider : terrainColliderList_)
	{
		if (const auto& col = collider.lock())
		{
			if (!col->GetEnabled())continue;
			col->UpdateWorldMatrix();
		}
	}

	for (const auto& collider : attackColliderList_)
	{
		if (const auto& col = collider.lock())
		{
			if (!col->GetEnabled())continue;
			col->UpdateWorldMatrix();
		}
	}

	for (const auto& collider : hitColliderList_)
	{
		if (const auto& col = collider.lock())
		{
			if (!col->GetEnabled())continue;
			col->UpdateWorldMatrix();
		}
	}
#endif // _DEBUG
}

void CollisionManager::TerrainDetection()
{
	for (const auto& collider1 : terrainColliderList_)
	{
		if (const auto& col1 = collider1.lock())
		{
			if (!col1->GetEnabled())continue;
			for (const auto& collider2 : terrainColliderList_)
			{
				if (const auto& col2 = collider2.lock())
				{
					if (!col2->GetEnabled())continue;
					if (col1 == col2)break;
					if (col1->GetTag() == col2->GetTag())break;

					//基準となるコライダーを判定
					SphereCollider* reci = col1.get();//受ける方のコライダー
					SphereCollider* push = col2.get();//押し出す方のコライダー

					if (col1->GetTag() > col2->GetTag())
					{
						reci = col2.get();
						push = col1.get();
					}

					//判定処理
					Collision::IntersectionResult result;
					if (!reci->IntersectVsSphere(*push, &result))break;

					

					//押し出す側の親アクターが存在しているか判定
					if (const auto& reciActor = reci->GetActor()->GetParent().lock())
					{
						if (const auto& pushActor = push->GetActor()->GetParent().lock())
						{
							//押し出し処理
							const Vector3 pushPos = push->GetTransform()->GetPosition();
							const Vector3 reciPos = reci->GetTransform()->GetPosition();
							const Vector3 normal = Vector3::Normalize(reciPos - pushPos);
							Vector3 vec = normal * (reci->GetRadius() + push->GetRadius());

							const Vector3 newReciPos = pushPos + vec;
							const Vector3 correctVec = newReciPos - reciPos;
							const Vector3 newPos = reciActor->GetTransform()->GetPosition() + correctVec;

							//位置を設定
							reciActor->GetTransform()->SetPosition(newPos);
						}
						
					}
				}
			}
		}
	}
}

void CollisionManager::AttackDetection()
{
	for (const auto& a : attackColliderList_)
	{
		if (const auto& atk = a.lock())
		{
			if (!atk->GetEnabled())continue;
			for (const auto& h : hitColliderList_)
			{
				if (const auto& hit = h.lock())
				{
					if (!hit->GetEnabled())continue;

					//タグが同じか
					if (atk->GetTag() == hit->GetTag())continue;

					//両方のコライダーを取得完了

					//当たり判定
					Collision::IntersectionResult result;
					if (atk->IntersectVsSphere(*hit.get(), &result))
					{
						OnCollision(atk, hit, result);
					}
				}
			}
		}
	}
}

void AbyssEngine::CollisionManager::RegisterTerrainCollider(const std::shared_ptr<SphereCollider>& collider)
{
	terrainColliderList_.emplace_back(collider);

}

void CollisionManager::RegisterAttackCollider(const std::shared_ptr<AttackCollider>& collider)
{
	attackColliderList_.emplace_back(collider);
}

void CollisionManager::RegisterHitCollider(const std::shared_ptr<HitCollider>& collider)
{
	hitColliderList_.emplace_back(collider);
}

void CollisionManager::DeleteAttackCollider(const std::shared_ptr<SphereCollider>& collider)
{
	//要素削除
	for (auto it = attackColliderList_.begin(); it != attackColliderList_.end(); it++)
	{
		if (const auto& atk = (*it).lock())
		{
			if (atk == collider)
			{
				attackColliderList_.erase(it);
				return;
			}
		}
	}
}

void CollisionManager::DeleteHitCollider(const std::shared_ptr<SphereCollider>& collider)
{
	//要素削除
	for (auto it = hitColliderList_.begin();it != hitColliderList_.end();it++)
	{
		if (const auto& hit = (*it).lock())
		{
			if (hit == collider)
			{
				hitColliderList_.erase(it);
				return;
			}
		}
	}
}

void CollisionManager::OnCollision(const std::shared_ptr<Collider>& myCollider, const std::shared_ptr<Collider>& collider, const Collision::IntersectionResult& result)
{
	//OnCollisionを呼んで判定が当たっていることを通知する
	//親オブジェクトのOnCollisionも呼んでいく

	std::shared_ptr<Actor> actor = myCollider->GetActor();

	while (true)
	{
		actor->OnCollision(collider,result);

		actor = actor->GetParent().lock();
		if (!actor)
		{
			break;
		}
	}
}