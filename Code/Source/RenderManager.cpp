#include "RenderManager.h"
#include "DXSystem.h"
#include "SpriteRenderer.h"
#include "Actor.h"
#include "Misc.h"

using namespace AbyssEngine;
using namespace std;

//void RenderManager::RenderMain()
//{
//    DXSystem::SetDefaultView();
//
//    //トポロジー設定
//    DXSystem::deviceContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//    
//    constexpr UINT stride = sizeof(Vertex);
//    constexpr UINT offset = 0;
//
//
//}

RenderManager::RenderManager()
{
	//サンプラーステート作成
	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = D3D11_FILTER_ANISOTROPIC;	  // 異方性フィルタ
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // U
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // V
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // W
	sd.MipLODBias = 0;
	sd.MaxAnisotropy = 4; // 最大異方性(1Pixelあたりのテクスチャ点数)
	sd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = DXSystem::device_->CreateSamplerState(&sd, sampler_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void RenderManager::Reset()
{
	for (auto& r : renderer2DList_)
	{
		if (const auto& rend = r.lock())
		{
			rend->isCalled_ = false;
		}
	}
	renderer2DList_.clear();
}

void RenderManager::Add(const shared_ptr<SpriteRenderer>& mRend)
{
	renderer2DList_.emplace_back(mRend);
}

void RenderManager::Render()
{
	CheckRenderer();

	Render2D();
}

void RenderManager::Render2D() const
{
	if (!renderer2DList_.empty())
	{
		//トポロジー設定
		DXSystem::deviceContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		for (auto& r : renderer2DList_)
		{
			const auto& pRend = r.lock();
			if (pRend->actor_->GetActiveInHierarchy())
			{
				if (pRend->GetEnabled())
				{
					pRend->Render();
				}
			}
		}
	}
}

void RenderManager::CheckRenderer()
{
	//2Dアクターの生存確認
	//参照先のshared_ptrが寿命切れしているなら要素を削除する
	bool expired = false;
	for (auto& r : renderer2DList_)
	{
		if (const auto& rend = r.lock())
		{
			rend->RecalculateFrame();
		}
		else
		{
			expired = true;
		}
	}
	if (expired)
	{
		const auto removeIt = remove_if(renderer2DList_.begin(), renderer2DList_.end(), [](weak_ptr<Renderer> r) { return r.expired(); });
		renderer2DList_.erase(removeIt, renderer2DList_.end());
	}
}
