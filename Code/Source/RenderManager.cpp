#include "RenderManager.h"
#include "DXSystem.h"
#include "Engine.h"
#include "SpriteRenderer.h"
#include "Actor.h"
#include "Misc.h"
#include "Camera.h"
#include "SkeletalMesh.h"
#include "GltfSkeletalMesh.h"
#include "FbxMeshData.h"
#include "Texture.h"
#include "GltfStaticMesh.h"
#include "StaticMesh.h"
#include "Bloom.h"
#include "Skybox.h"
#include "CascadedShadowMap.h"

#include "imgui/imgui.h"

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

#if 0
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ConstantBufferScene);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		const HRESULT hr = DXSystem::device_->CreateBuffer(&bd, nullptr, constantBufferScene_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
#else
		//シーン用
		bufferScene_ = std::make_unique<ConstantBuffer<ConstantBufferScene>>();

		//エフェクト用
		bufferEffects_ = std::make_unique<ConstantBuffer<ConstantEffects>>();
#endif // 0
	}

	{
		//サンプラーステート作成
#if 0
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
		HRESULT hr = DXSystem::device_->CreateSamplerState(&sd, samplers_[static_cast<size_t>(SP_State::Anisotropic)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));


		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		hr = DXSystem::device_->CreateSamplerState(&sd, samplers_[static_cast<size_t>(SP_State::Point)].GetAddressOf());

		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		hr = DXSystem::device_->CreateSamplerState(&sd, samplers_[static_cast<size_t>(SP_State::Linear)].GetAddressOf());

		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.BorderColor[0] = 0;
		sd.BorderColor[1] = 0;
		sd.BorderColor[2] = 0;
		sd.BorderColor[3] = 0;
		hr = DXSystem::device_->CreateSamplerState(&sd, samplers_[static_cast<size_t>(SP_State::LinearBorderBlack)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.BorderColor[0] = 1;
		sd.BorderColor[1] = 1;
		sd.BorderColor[2] = 1;
		sd.BorderColor[3] = 1;
		hr = DXSystem::device_->CreateSamplerState(&sd, samplers_[static_cast<size_t>(SP_State::LinearBorderWhite)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
#else
		D3D11_SAMPLER_DESC sd[]{
		{D3D11_FILTER_MIN_MAG_MIP_POINT/*Filter*/, D3D11_TEXTURE_ADDRESS_WRAP/*AddressU*/, D3D11_TEXTURE_ADDRESS_WRAP/*AddressV*/, D3D11_TEXTURE_ADDRESS_WRAP/*AddressW*/, 0.0f/*MipLODBias*/, 0/*MaxAnisotropy*/, D3D11_COMPARISON_NEVER/*ComparisonFunc*/, {0, 0, 0, 0}/*BorderColor[4]*/, 0.0f/*MinLOD*/, D3D11_FLOAT32_MAX/*MaxLOD*/},
		{D3D11_FILTER_MIN_MAG_MIP_LINEAR/*Filter*/, D3D11_TEXTURE_ADDRESS_WRAP/*AddressU*/, D3D11_TEXTURE_ADDRESS_WRAP/*AddressV*/, D3D11_TEXTURE_ADDRESS_WRAP/*AddressW*/, 0.0f/*MipLODBias*/, 0/*MaxAnisotropy*/, D3D11_COMPARISON_NEVER/*ComparisonFunc*/, {0, 0, 0, 0}/*BorderColor[4]*/, 0.0f/*MinLOD*/, D3D11_FLOAT32_MAX/*MaxLOD*/},
		{D3D11_FILTER_ANISOTROPIC/*Filter*/, D3D11_TEXTURE_ADDRESS_WRAP/*AddressU*/, D3D11_TEXTURE_ADDRESS_WRAP/*AddressV*/, D3D11_TEXTURE_ADDRESS_WRAP/*AddressW*/, 0.0f/*MipLODBias*/, 8/*MaxAnisotropy*/, D3D11_COMPARISON_NEVER/*ComparisonFunc*/, {0, 0, 0, 0}/*BorderColor[4]*/, 0.0f/*MinLOD*/, D3D11_FLOAT32_MAX/*MaxLOD*/},
		{D3D11_FILTER_MIN_MAG_MIP_POINT/*Filter*/, D3D11_TEXTURE_ADDRESS_CLAMP/*AddressU*/, D3D11_TEXTURE_ADDRESS_CLAMP/*AddressV*/, D3D11_TEXTURE_ADDRESS_CLAMP/*AddressW*/, 0.0f/*MipLODBias*/, 0/*MaxAnisotropy*/, D3D11_COMPARISON_NEVER/*ComparisonFunc*/, {0, 0, 0, 0}/*BorderColor[4]*/, 0.0f/*MinLOD*/, D3D11_FLOAT32_MAX/*MaxLOD*/},
		{D3D11_FILTER_MIN_MAG_MIP_LINEAR/*Filter*/, D3D11_TEXTURE_ADDRESS_CLAMP/*AddressU*/, D3D11_TEXTURE_ADDRESS_CLAMP/*AddressV*/, D3D11_TEXTURE_ADDRESS_CLAMP/*AddressW*/, 0.0f/*MipLODBias*/, 0/*MaxAnisotropy*/, D3D11_COMPARISON_NEVER/*ComparisonFunc*/, {0, 0, 0, 0}/*BorderColor[4]*/, 0.0f/*MinLOD*/, D3D11_FLOAT32_MAX/*MaxLOD*/},
		{D3D11_FILTER_ANISOTROPIC/*Filter*/, D3D11_TEXTURE_ADDRESS_CLAMP/*AddressU*/, D3D11_TEXTURE_ADDRESS_CLAMP/*AddressV*/, D3D11_TEXTURE_ADDRESS_CLAMP/*AddressW*/, 0.0f/*MipLODBias*/, 8/*MaxAnisotropy*/, D3D11_COMPARISON_NEVER/*ComparisonFunc*/, {0, 0, 0, 0}/*BorderColor[4]*/, 0.0f/*MinLOD*/, D3D11_FLOAT32_MAX/*MaxLOD*/},
		{D3D11_FILTER_MIN_MAG_MIP_POINT/*Filter*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressU*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressV*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressW*/, 0.0f/*MipLODBias*/, 0/*MaxAnisotropy*/, D3D11_COMPARISON_NEVER/*ComparisonFunc*/, {0, 0, 0, 1}/*BorderColor[4]*/, 0.0f/*MinLOD*/, D3D11_FLOAT32_MAX/*MaxLOD*/},
		{D3D11_FILTER_MIN_MAG_MIP_LINEAR/*Filter*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressU*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressV*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressW*/, 0.0f/*MipLODBias*/, 0/*MaxAnisotropy*/, D3D11_COMPARISON_NEVER/*ComparisonFunc*/, {0, 0, 0, 1}/*BorderColor[4]*/, 0.0f/*MinLOD*/, D3D11_FLOAT32_MAX/*MaxLOD*/},
		{D3D11_FILTER_MIN_MAG_MIP_POINT/*Filter*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressU*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressV*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressW*/, 0.0f/*MipLODBias*/, 0/*MaxAnisotropy*/, D3D11_COMPARISON_NEVER/*ComparisonFunc*/, {1, 1, 1, 1}/*BorderColor[4]*/, 0.0f/*MinLOD*/, D3D11_FLOAT32_MAX/*MaxLOD*/},
		{D3D11_FILTER_MIN_MAG_MIP_LINEAR/*Filter*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressU*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressV*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressW*/, 0.0f/*MipLODBias*/, 0/*MaxAnisotropy*/, D3D11_COMPARISON_NEVER/*ComparisonFunc*/, {1, 1, 1, 1}/*BorderColor[4]*/, 0.0f/*MinLOD*/, D3D11_FLOAT32_MAX/*MaxLOD*/},
		{D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT/*Filter*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressU*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressV*/, D3D11_TEXTURE_ADDRESS_BORDER/*AddressW*/, 0.0f/*MipLODBias*/, 16/*MaxAnisotropy*/, D3D11_COMPARISON_LESS_EQUAL/*ComparisonFunc*/, {1, 1, 1, 1}/*BorderColor[4]*/, 0.0f/*MinLOD*/, D3D11_FLOAT32_MAX/*MaxLOD*/},
		};
		for (size_t samplerIndex = 0; samplerIndex < _countof(sd); ++samplerIndex)
		{
			const HRESULT hr = DXSystem::device_->CreateSamplerState(&sd[samplerIndex], samplers_[samplerIndex].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
#endif // 0
	}

	IBLInitialize();

	//オフスクリーンレンダリング
	baseFrameBuffer_ = std::make_unique<FrameBuffer>(DXSystem::GetScreenWidth(), DXSystem::GetScreenHeight());
	bitBlockTransfer_ = std::make_unique<FullscreenQuad>();
	bloom_ = std::make_unique<Bloom>(
		static_cast<size_t>(DXSystem::GetScreenWidth()), 
		static_cast<size_t>(DXSystem::GetScreenHeight()));
#if 0
	postEffectsPS_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/FinalPassPS.cso");
#else
	postEffectsPS_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/PostEffectsPS.cso");
#endif // 0

	//スカイボックス生成
	skybox_ = std::make_unique<Skybox>();

	//シャドウマップ生成
	cascadedShadowMap_ = std::make_unique<CascadedShadowMap>(1024.0f * 4,1024.0f * 4);
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

void RenderManager::Add(const shared_ptr<SkeletalMesh>& mRend)
{
	renderer3DList_.emplace_back(mRend);
}

//void RenderManager::Add(const shared_ptr<GltfSkeletalMesh>& mRend)
//{
//	renderer3DList_.emplace_back(mRend);
//}

void RenderManager::Add(const shared_ptr<StaticMesh>& mRend)
{
	renderer3DList_.emplace_back(mRend);
}

void RenderManager::Add(const shared_ptr<Camera>& camera_)
{
	cameraList_.emplace_back(camera_);
}

void RenderManager::Render()
{
	DXSystem::SetDefaultView();

	IBLSetResources();

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

				//シーン用定数バッファの設定
				Vector3 const eye = camera->eye_;
				Vector3 const focus = camera->focus_;
				bufferScene_->data_.eyePosition_ = Vector4(eye.x,eye.y,eye.z,1.0f);
				bufferScene_->data_.focusPosition_ = Vector4(focus.x, focus.y, focus.z,1.0f);
				bufferScene_->data_.view_ = camera->viewMatrix_;
				bufferScene_->data_.projection_ = camera->projectionMatrix_;
				bufferScene_->data_.viewProjectionMatrix_ = camera->viewProjectionMatrix_;
				camera->projectionMatrix_.Invert(bufferScene_->data_.inverseProjection_);
				camera->viewProjectionMatrix_.Invert(bufferScene_->data_.inverseViewProjection_);
				bufferScene_->data_.time_ += Time::deltaTime_;


				//定数バッファ更新
#if 0
				UpdateConstantBuffer();
				DXSystem::deviceContext_->VSSetConstantBuffers(0, 1, constantBufferScene_.GetAddressOf());
				DXSystem::deviceContext_->PSSetConstantBuffers(0, 1, constantBufferScene_.GetAddressOf());
#else
				bufferScene_->Activate(10, CBufferUsage::vp);
#endif // 0

				//シャドウマップ作成
				cascadedShadowMap_->Clear();
				if (enableShadow_)
				{
					DXSystem::SetDepthStencilState(DS_State::LEqual,0);
					DXSystem::SetRasterizerState(RS_State::Cull_None);
					DXSystem::SetBlendState(BS_State::Off);
					cascadedShadowMap_->Make(
						bufferScene_->data_.view_,
						bufferScene_->data_.projection_,
						bufferScene_->data_.lightDirection_,
						criticalDepthValue_,
						[&]() {
							for (auto& r : renderer3DList_)
							{
								const auto& pRend = r.lock();
								if (pRend->actor_->GetActiveInHierarchy())
								{
									if (pRend)
									{
										pRend->RenderShadow();
									}
								}
							}
						}
					);
				}

				//仮のライト
				//bufferScene_.lightDirection_ = Vector4(0, 0, 1, 0);
				//bufferScene_.lightColor_ = Vector3(1, 1, 1);


				//オフスクリーンレンダリング
				baseFrameBuffer_->Clear(0.4f,0.4f,0.4f,1.0f);
				baseFrameBuffer_->Activate();

				//スカイボックス描画
				skybox_->Render(bitBlockTransfer_.get());

				Render3D(camera);

				baseFrameBuffer_->Deactivate();
				bloom_->Make(baseFrameBuffer_->shaderResourceViews_[0].Get());

				DXSystem::deviceContext_->PSSetShaderResources(39, 1, baseFrameBuffer_->GetColorMap().GetAddressOf());

#if 0
				ID3D11ShaderResourceView* shaderResourceViews[] =
				{
					baseFrameBuffer_->shaderResourceViews_[0].Get(),
					bloom_->GetShaderResourceView(),
				};
				bitBlockTransfer_->Blit(shaderResourceViews, 0, 2, postEffectsPS_.Get());
#else
				bufferEffects_->Activate(11, CBufferUsage::vp);
				DXSystem::SetDepthStencilState(DS_State::None);
				DXSystem::SetBlendState(BS_State::Off);
				DXSystem::SetRasterizerState(RS_State::Cull_None);
				ID3D11ShaderResourceView* shaderResourceViews[] =
				{
					baseFrameBuffer_->GetColorMap().Get(),
					baseFrameBuffer_->GetDepthMap().Get(),
					cascadedShadowMap_->DepthMap().Get(),
					bloom_->GetShaderResourceView()
				};
				bitBlockTransfer_->Blit(shaderResourceViews, 0, _countof(shaderResourceViews), postEffectsPS_.Get());
#endif // 0
				break;
			}
		}
	}

	Render2D();
}

void RenderManager::DrawImGui()
{
	if (ImGui::BeginMenu("Scene Constant"))
	{
		auto& buffer = bufferScene_->data_;
		ImGui::DragFloat3("Light Direction",&buffer.lightDirection_.x,0.01f);
		//ImGui::ColorEdit4("Light Color", &buffer.lightColor_.x, ImGuiColorEditFlags_HDR);

		ImGui::DragFloat("Exposure", &buffer.exposure_,0.01f,0.0f);
		ImGui::DragFloat("Pure White", &buffer.pureWhite_,0.01f,0.0f);
		ImGui::DragFloat("Emissive Intensity", &buffer.emissiveIntensity_,0.01f,0.0f);
		ImGui::DragFloat("Image Based Lighting Intensity", &buffer.imageBasedLightingIntensity_,0.01f,0.0f);
		ImGui::DragFloat("Skybox Roughness", &buffer.skyboxRoughness_,0.01f,0.0f);
		ImGui::DragFloat("Time", &buffer.time_);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Post Effect"))
	{
		bloom_->DrawImGui();

		ImGui::Checkbox("Enable Shadow", &enableShadow_);
		ImGui::Checkbox("Colorize Cascaded Layer", reinterpret_cast<bool*>(&bufferEffects_->data_.colorizeCascadedLayer_));
		ImGui::DragFloat("Split Scheme Weight", &cascadedShadowMap_->splitSchemeWeight_, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Critical Depth Value", &criticalDepthValue_, 1.0f, 0.0f, 1000.0f);
		ImGui::DragFloat("Shadow Depth Bias", &bufferEffects_->data_.shadowDepthBias_, 0.000001f, 0.0f, 0.01f, "%.8f");
		ImGui::SliderFloat("Shadow Color", &bufferEffects_->data_.shadowColor_, 0.0f, 1.0f);
		ImGui::SliderFloat("Shadow Filter Radius", &bufferEffects_->data_.shadowFilterRadius_, 0.0f, 64.0f);
		ImGui::SliderInt("Shadow Sample Count", reinterpret_cast<int*>(&bufferEffects_->data_.shadowSampleCount_), 0, 64);

		ImGui::EndMenu();
	}
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

void RenderManager::Render3D(const shared_ptr<Camera>& camera_)
{
	//トポロジー設定
	//DXSystem::deviceContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	DXSystem::SetBlendState(BS_State::Off);
	DXSystem::SetDepthStencilState(DS_State::LEqual);
	DXSystem::SetRasterizerState(RS_State::Cull_None);

#if 0

	DXSystem::deviceContext_->PSSetSamplers(0, 1, samplers_[static_cast<int>(SP_State::Anisotropic)].GetAddressOf());
	DXSystem::deviceContext_->PSSetSamplers(1, 1, samplers_[static_cast<int>(SP_State::Point)].GetAddressOf());
	DXSystem::deviceContext_->PSSetSamplers(2, 1, samplers_[static_cast<int>(SP_State::Linear)].GetAddressOf());
	DXSystem::deviceContext_->PSSetSamplers(3, 1, samplers_[static_cast<int>(SP_State::LinearBorderBlack)].GetAddressOf());
	DXSystem::deviceContext_->PSSetSamplers(4, 1, samplers_[static_cast<int>(SP_State::LinearBorderWhite)].GetAddressOf());
#else
	for (size_t samplerIndex = 0; samplerIndex < _countof(samplers_); ++samplerIndex)
	{
		DXSystem::deviceContext_->PSSetSamplers(static_cast<UINT>(samplerIndex), 1, samplers_[samplerIndex].GetAddressOf());
	}
#endif // 0


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
#if 0
	constexpr UINT subresourceIndex = 0;
	//D3D11_MAPPED_SUBRESOURCE mapped;

	DXSystem::deviceContext_->UpdateSubresource(constantBufferScene_.Get(),subresourceIndex, 0, &bufferScene_, 0,0);
	/*const auto hr = DXSystem::deviceContext_->Map(constantBufferScene_.Get(),subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, &bufferScene_, sizeof(constantBufferScene_));
		DXSystem::deviceContext_->Unmap(constantBufferScene_.Get(), subresourceIndex);
	}*/
#endif // 0
}

void RenderManager::IBLInitialize()
{                                 
	D3D11_TEXTURE2D_DESC texture2dDesc{};
	texture2dDesc.Width = DXSystem::GetScreenWidth();
	texture2dDesc.Height = DXSystem::GetScreenHeight();
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2dDesc.CPUAccessFlags = 0;
	texture2dDesc.MiscFlags = 0;
#if 0
	Texture::LoadTextureFromFile("./Assets/dds/sunset_jhbcentral_4k/sunset_jhbcentral_4k.dds", iblShaderResourceView_[0].GetAddressOf(), &texture2dDesc);
	Texture::LoadTextureFromFile("./Assets/dds/sunset_jhbcentral_4k/diffuse_iem.dds", iblShaderResourceView_[1].GetAddressOf(), &texture2dDesc);
	Texture::LoadTextureFromFile("./Assets/dds/sunset_jhbcentral_4k/specular_pmrem.dds", iblShaderResourceView_[2].GetAddressOf(), &texture2dDesc);
	Texture::LoadTextureFromFile("./Assets/dds/lut_ggx.dds", iblShaderResourceView_[3].GetAddressOf(), &texture2dDesc);
#else
	//Texture::LoadTextureFromFile("./Assets/dds/syferfontein_0d_clear_puresky_4k/syferfontein_0d_clear_puresky_4k.dds", iblShaderResourceView_[0].GetAddressOf(), &texture2dDesc);
	Texture::LoadTextureFromFile("./Assets/dds/kloofendal_48d_partly_cloudy_puresky_4k/kloofendal_48d_partly_cloudy_puresky_4k.dds", iblShaderResourceView_[0].GetAddressOf(), &texture2dDesc);
	Texture::LoadTextureFromFile("./Assets/dds/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds", iblShaderResourceView_[1].GetAddressOf(), &texture2dDesc);
	Texture::LoadTextureFromFile("./Assets/dds/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds", iblShaderResourceView_[2].GetAddressOf(), &texture2dDesc);
	Texture::LoadTextureFromFile("./Assets/dds/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds", iblShaderResourceView_[3].GetAddressOf(), &texture2dDesc);
	Texture::LoadTextureFromFile("./Assets/dds/lut_ggx.dds", iblShaderResourceView_[4].GetAddressOf(), &texture2dDesc);
	Texture::LoadTextureFromFile("./Assets/dds/lut_sheen_E.dds", iblShaderResourceView_[5].GetAddressOf(), &texture2dDesc);
	Texture::LoadTextureFromFile("./Assets/dds/lut_charlie.dds", iblShaderResourceView_[6].GetAddressOf(), &texture2dDesc);
#endif // 0

	
}

void RenderManager::IBLSetResources()
{
	DXSystem::deviceContext_->PSSetShaderResources(32, 1, iblShaderResourceView_[0].GetAddressOf());
	DXSystem::deviceContext_->PSSetShaderResources(33, 1, iblShaderResourceView_[1].GetAddressOf());
	DXSystem::deviceContext_->PSSetShaderResources(34, 1, iblShaderResourceView_[2].GetAddressOf());
	DXSystem::deviceContext_->PSSetShaderResources(35, 1, iblShaderResourceView_[3].GetAddressOf());
	DXSystem::deviceContext_->PSSetShaderResources(36, 1, iblShaderResourceView_[4].GetAddressOf());
	DXSystem::deviceContext_->PSSetShaderResources(37, 1, iblShaderResourceView_[5].GetAddressOf());
	DXSystem::deviceContext_->PSSetShaderResources(38, 1, iblShaderResourceView_[6].GetAddressOf());
}
