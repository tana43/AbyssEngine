#include "CollisionManager.h"


void AbyssEngine::CollisionManager::TerrainDetection()
{
	for (const auto& collider1 : terrainColliderList_)
	{
		if (const auto& col1 = collider1.lock())
		{
			if (!col1->GetEnabled())break;
			for (const auto& collider2 : terrainColliderList_)
			{
				if (const auto& col2 = collider2.lock())
				{
					if (!col1->GetEnabled())break;
					if (col1 == col2)break;

					//���菈��


				}
			}
		}
	}
}

void AbyssEngine::CollisionManager::AttackDetection()
{
	for (const auto& a : attackColliderList_)
	{
		if (const auto& atk = a.lock())
		{
			if (!atk->GetEnabled())break;
			for (const auto& h : hitColliderList_)
			{
				if (const auto& hit = h.lock())
				{
					if (!hit->GetEnabled())
					{
						//�����̃R���C�_�[���擾����

						//�����蔻��
						atk->OnCollision(hit);
					}
				}
			}
		}
		
	}
}
