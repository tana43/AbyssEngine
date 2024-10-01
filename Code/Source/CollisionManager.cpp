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
					if (!col1->GetEnabled())continue;
					if (col1 == col2)break;

					//判定処理


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
					if (atk->IntersectVsSphere(hit,&result))
					{
						OnCollision(atk, hit, result);
					}
				}
			}
		}
	}
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