#include "RenderManager.h"
#include "DXSystem.h"
#include "SpriteRenderer.h"
#include "Actor.h"
#include "Misc.h"
#include "Camera.h"
#include "SkeltalMesh.h"
#include "MeshData.h"

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
	{
		//定数バッファ作成
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ConstantBufferScene);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		const HRESULT hr = DXSystem::device_->CreateBuffer(&bd, nullptr, constantBufferScene_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

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
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}
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

void RenderManager::Add(const shared_ptr<SkeltalMesh>& mRend)
{
	renderer3DList_.emplace_back(mRend);
}

void RenderManager::Add(const shared_ptr<Camera>& camera)
{
	cameraList_.emplace_back(camera);
}

void RenderManager::Render()
{
	DXSystem::SetDefaultView();

	CheckRenderer();

	for (auto& c : cameraList_)
	{
		if (const auto& camera = c.lock())
		{
			if (camera->actor_->GetActiveInHierarchy())
			{
				camera->Update();


				const Vector3& pos = camera->GetTransform()->GetPosition();
				//const Vector3& dir = camera->GetTransform()->GetForward();

				bufferScene_.cameraPosition_ = Vector4(pos.x,pos.y,pos.z,0);
				bufferScene_.viewProjectionMatrix_ = camera->viewProjectionMatrix_;

				//仮のライト
				bufferScene_.lightDirection_ = Vector4(0, 0, 1, 0);
				bufferScene_.lightColor_ = Vector3(1, 1, 1);

				UpdateConstantBuffer();

				DXSystem::deviceContext_->VSSetConstantBuffers(0, 1, constantBufferScene_.GetAddressOf());
				DXSystem::deviceContext_->PSSetConstantBuffers(0, 1, constantBufferScene_.GetAddressOf());

				Render3D(camera);

				break;
			}
		}
	}

	Render2D();
}

void RenderManager::Render2D() const
{
	if (!renderer2DList_.empty())
	{
		//トポロジー設定
		DXSystem::deviceContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		DXSystem::deviceContext_->OMSetBlendState(DXSystem::GetBlendState(BS_State::Alpha),nullptr,0xFFFFFFFF);
		DXSystem::deviceContext_->OMSetDepthStencilState(DXSystem::GetDepthStencilState(DS_State::None_No_Write),0);
		DXSystem::deviceContext_->RSSetState(DXSystem::GetRasterizerState(RS_State::Cull_None));

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

void RenderManager::Render3D(const shared_ptr<Camera>& camera)
{
	//トポロジー設定
	//DXSystem::deviceContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DXSystem::deviceContext_->OMSetBlendState(DXSystem::GetBlendState(BS_State::Off), nullptr, 0xFFFFFFFF);
	DXSystem::deviceContext_->OMSetDepthStencilState(DXSystem::GetDepthStencilState(DS_State::LEqual), 1);
	DXSystem::deviceContext_->RSSetState(DXSystem::GetRasterizerState(RS_State::Cull_Back));

	DXSystem::deviceContext_->PSSetSamplers(0,1,sampler_.GetAddressOf());

	for (auto& r : renderer3DList_)
	{
		const auto& pRend = r.lock();
		if (pRend->actor_->GetActiveInHierarchy())
		{
			if (pRend)
			{
				pRend->Render();
			}
		}
	}
}

void RenderManager::CheckRenderer()
{
	//3Dアクター生存確認
	{
		bool expired = false;
		for (auto& r : renderer3DList_)
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
			const auto removeIt = remove_if(renderer3DList_.begin(), renderer3DList_.end(), [](auto& r) {return r.expired(); });
			renderer3DList_.erase(removeIt, renderer3DList_.end());
		}
	}

	//2Dアクターの生存確認
	//参照先のshared_ptrが寿命切れしているなら要素を削除する
	{
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
}

void RenderManager::UpdateConstantBuffer() const
{
	constexpr UINT subresourceIndex = 0;
	//D3D11_MAPPED_SUBRESOURCE mapped;

	DXSystem::deviceContext_->UpdateSubresource(constantBufferScene_.Get(),subresourceIndex, 0, &bufferScene_, 0,0);
	/*const auto hr = DXSystem::deviceContext_->Map(constantBufferScene_.Get(),subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, &bufferScene_, sizeof(constantBufferScene_));
		DXSystem::deviceContext_->Unmap(constantBufferScene_.Get(), subresourceIndex);
	}*/
}
