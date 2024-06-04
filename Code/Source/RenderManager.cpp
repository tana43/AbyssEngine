#include "RenderManager.h"
#include "DXSystem.h"
#include "Engine.h"
#include "Actor.h"
#include "Misc.h"
#include "Camera.h"

#include "SpriteRenderer.h"
#include "Texture.h"
#include "SkeletalMesh.h"
#include "StaticMesh.h"

#include "Bloom.h"
#include "Skybox.h"
#include "CascadedShadowMap.h"

#include "Keyboard.h"

#include "EffectManager.h"

#if _DEBUG
#include "DebugRenderer.h"
#include "LineRenderer.h"
#endif // _DEBUG

#include "imgui/imgui.h"

using namespace AbyssEngine;
using namespace std;

//void RenderManager::RenderMain()
//{
//    DXSystem::SetDefaultView();
//
//    //�g�|���W�[�ݒ�
//    DXSystem::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//    
//    constexpr UINT stride = sizeof(Vertex);
//    constexpr UINT offset = 0;
//
//
//}

RenderManager::RenderManager()
{
	{
		//�萔�o�b�t�@�쐬

#if 0
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ConstantBufferScene);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		const HRESULT hr = DXSystem::GetDevice()->CreateBuffer(&bd, nullptr, constantBufferScene_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
#else
		//�V�[���p
		bufferScene_ = std::make_unique<ConstantBuffer<ConstantBufferScene>>();

		//�G�t�F�N�g�p
		bufferEffects_ = std::make_unique<ConstantBuffer<ConstantEffects>>();
#endif // 0
	}

	{
		//�T���v���[�X�e�[�g�쐬
#if 0
		D3D11_SAMPLER_DESC sd = {};
		sd.Filter = D3D11_FILTER_ANISOTROPIC;	  // �ٕ����t�B���^
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // U
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // V
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // W
		sd.MipLODBias = 0;
		sd.MaxAnisotropy = 4; // �ő�ٕ���(1Pixel������̃e�N�X�`���_��)
		sd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
		HRESULT hr = DXSystem::GetDevice()->CreateSamplerState(&sd, samplers_[static_cast<size_t>(SP_State::Anisotropic)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));


		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		hr = DXSystem::GetDevice()->CreateSamplerState(&sd, samplers_[static_cast<size_t>(SP_State::Point)].GetAddressOf());

		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		hr = DXSystem::GetDevice()->CreateSamplerState(&sd, samplers_[static_cast<size_t>(SP_State::Linear)].GetAddressOf());

		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.BorderColor[0] = 0;
		sd.BorderColor[1] = 0;
		sd.BorderColor[2] = 0;
		sd.BorderColor[3] = 0;
		hr = DXSystem::GetDevice()->CreateSamplerState(&sd, samplers_[static_cast<size_t>(SP_State::LinearBorderBlack)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.BorderColor[0] = 1;
		sd.BorderColor[1] = 1;
		sd.BorderColor[2] = 1;
		sd.BorderColor[3] = 1;
		hr = DXSystem::GetDevice()->CreateSamplerState(&sd, samplers_[static_cast<size_t>(SP_State::LinearBorderWhite)].GetAddressOf());
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
			const HRESULT hr = DXSystem::GetDevice()->CreateSamplerState(&sd[samplerIndex], samplers_[samplerIndex].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
#endif // 0
	}

	IBLInitialize();

	//�I�t�X�N���[�������_�����O
	baseFrameBuffer_[0] = std::make_unique<FrameBuffer>(DXSystem::GetScreenWidth(), DXSystem::GetScreenHeight());
	baseFrameBuffer_[1] = std::make_unique<FrameBuffer>(DXSystem::GetScreenWidth(), DXSystem::GetScreenHeight(),false,true);
	postEffectedFrameBuffer_ = std::make_unique<FrameBuffer>(DXSystem::GetScreenWidth(), DXSystem::GetScreenHeight(),false);
	bitBlockTransfer_ = std::make_unique<FullscreenQuad>();
	bloom_ = std::make_unique<Bloom>(
		static_cast<size_t>(DXSystem::GetScreenWidth()), 
		static_cast<size_t>(DXSystem::GetScreenHeight()));
#if 0
	postEffectsPS_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/FinalPassPS.cso");
#else
	postEffectsPS_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/PostEffectsPS.cso");
#endif // 0

	toneMapPS_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/ToneMapPS.cso");

	//�X�J�C�{�b�N�X����
	skybox_ = std::make_unique<Skybox>();

	//�V���h�E�}�b�v����
	cascadedShadowMap_ = std::make_unique<CascadedShadowMap>(1024.0f * 4,1024.0f * 4);

	//�f�o�b�O�p�����_���[����
	debugRenderer_ = std::make_unique<DebugRenderer>(DXSystem::GetDevice());
	lineRenderer_ = std::make_unique<LineRenderer>(DXSystem::GetDevice(),1024);

	//�G�t�F�N�V�A������
	EffectManager::Instance().Initialize();

	//G-Buffer����
#if ENABLE_DIFFERD_RENDERING
	{
		HRESULT hr = S_OK;

		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = DXSystem::GetScreenWidth();
		texture2dDesc.Height = DXSystem::GetScreenHeight();
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;

		DXGI_FORMAT formats[] =
		{
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_R32_FLOAT,
		};
		for (int i = GB_BaseColor; i < GB_Max; ++i)
		{
			texture2dDesc.Format = formats[i];

			Microsoft::WRL::ComPtr<ID3D11Texture2D> color_buffer{};
			hr = DXSystem::GetDevice()->CreateTexture2D(&texture2dDesc, NULL, color_buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
			//	�����_�[�^�[�Q�b�g�r���[����
			hr = DXSystem::GetDevice()->CreateRenderTargetView(color_buffer.Get(), NULL, gBufferRenderTargetView_[i].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
			//	�V�F�[�_�[���\�[�X�r���[����
			hr = DXSystem::GetDevice()->CreateShaderResourceView(color_buffer.Get(), NULL, gBufferShaderResourceView_[i].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
	}

	//�V�F�[�_�[�̓ǂݍ���
	{
	 
	}

#endif // ENABLE_DIFFERD_RENDERING
}

RenderManager::~RenderManager()
{
	EffectManager::Instance().Finalize();
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

void RenderManager::Add(const shared_ptr<Camera>& camera)
{
	cameraList_.emplace_back(camera);
	if (cameraList_.size() == 1)ChangeMainCamera(camera.get());
}

void RenderManager::Render()
{
	DXSystem::SetDefaultView();

	IBLSetResources();

	CheckRenderer();

	//�G�t�F�N�g�X�V����
	EffectManager::Instance().Update(Time::deltaTime_);

	for (auto& c : cameraList_)
	{
		if (const auto& camera = c.lock())
		{
			if (camera->actor_->GetActiveInHierarchy())
			{
				if (!camera->GetIsMainCamera())continue;

				camera->Update();

				const Vector3& pos = camera->GetTransform()->GetPosition();
				//const Vector3& dir = camera->GetTransform()->GetForward();

				//�V�[���p�萔�o�b�t�@�̐ݒ�
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


				//�萔�o�b�t�@�X�V
#if 0
				UpdateConstantBuffer();
				DXSystem::GetDeviceContext()->VSSetConstantBuffers(0, 1, constantBufferScene_.GetAddressOf());
				DXSystem::GetDeviceContext()->PSSetConstantBuffers(0, 1, constantBufferScene_.GetAddressOf());
#else
				bufferScene_->Activate(10, CBufferUsage::vp);
#endif // 0

				//�V���h�E�}�b�v�쐬
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

				//���̃��C�g
				//bufferScene_.lightDirection_ = Vector4(0, 0, 1, 0);
				//bufferScene_.lightColor_ = Vector3(1, 1, 1);


#if ENABLE_DIFFERD_RENDERING 
				//�o�͐��GBuffer�ɕύX
				ID3D11RenderTargetView* renderTargets[GB_Max] =
				{
					gBufferRenderTargetView_[GB_BaseColor].Get(),
					gBufferRenderTargetView_[GB_Emissive].Get(),
					gBufferRenderTargetView_[GB_Normal].Get(),
					gBufferRenderTargetView_[GB_Parameters].Get(),
					gBufferRenderTargetView_[GB_Depth].Get()
				};
				FLOAT clearColor[] = { 0.0f,0.0f,0.0f,0.0f };
				for (int i = GB_BaseColor; i < GB_Max; i++)
				{
					DXSystem::GetDeviceContext()->ClearRenderTargetView(renderTargets[i], clearColor);
				}
				DXSystem::GetDeviceContext()->OMSetRenderTargets(GB_Max, renderTargets, DXSystem::GetDepthStencilView());
#else
				//�I�t�X�N���[�������_�����O
				baseFrameBuffer_[0]->Clear(0.4f, 0.4f, 0.4f, 1.0f);
				baseFrameBuffer_[0]->Activate();
#endif // ENABLE_DIFFERD_RENDERING 
				
				//�X�J�C�{�b�N�X�`��
				skybox_->Render(bitBlockTransfer_.get());

				//3D�I�u�W�F�N�g�`��
				Render3D(camera);

				//3D�G�t�F�N�g�`��
				EffectManager::Instance().Render(camera->viewMatrix_, camera->projectionMatrix_);

#if _DEBUG
				//�f�o�b�O�����_���[
				if (Keyboard::GetKeyDown(DirectX::Keyboard::D5))
				{
					enableDebugRender_ = !enableDebugRender_;
				}
				if (enableDebugRender_)
				{
					debugRenderer_->Render(DXSystem::GetDeviceContext(), camera->viewMatrix_, camera->projectionMatrix_);
					lineRenderer_->Render(DXSystem::GetDeviceContext(), camera->viewMatrix_, camera->projectionMatrix_);
				}
#endif // _DEBUG

				baseFrameBuffer_[0]->Deactivate();

				//�t���[���o�b�t�@�[���R�s�[
				baseFrameBuffer_[1]->CopyFrom(baseFrameBuffer_[0].get());
				baseFrameBuffer_[1]->GenerateMips();//�჌�x���̃~�b�v�}�b�v�̐���
				

				bloom_->Make(baseFrameBuffer_[0]->GetColorMap().Get());

				DXSystem::GetDeviceContext()->PSSetShaderResources(39, 1, baseFrameBuffer_[1]->GetColorMap().GetAddressOf());

				//PostEffect
				{
#if 0
					ID3D11ShaderResourceView* shaderResourceViews[] =
					{
						baseFrameBuffer_->shaderResourceViews_[0].Get(),
						bloom_->GetShaderResourceView(),
					};
					bitBlockTransfer_->Blit(shaderResourceViews, 0, 2, postEffectsPS_.Get());
#else
					postEffectedFrameBuffer_->Activate();

					bufferEffects_->Activate(11, CBufferUsage::vp);
					DXSystem::SetDepthStencilState(DS_State::None);
					DXSystem::SetBlendState(BS_State::Off);
					DXSystem::SetRasterizerState(RS_State::Cull_None);
					ID3D11ShaderResourceView* shaderResourceViews[] =
					{
						baseFrameBuffer_[0]->GetColorMap().Get(),
						baseFrameBuffer_[0]->GetDepthMap().Get(),
						cascadedShadowMap_->DepthMap().Get(),
						bloom_->GetShaderResourceView()
					};
					bitBlockTransfer_->Blit(shaderResourceViews, 0, _countof(shaderResourceViews), postEffectsPS_.Get());



					postEffectedFrameBuffer_->Deactivate();
#endif // 0
				}

				//Tone Mapping
				{
					DXSystem::SetDepthStencilState(DS_State::LEqual,0);
					DXSystem::SetBlendState(BS_State::Off);
					DXSystem::SetRasterizerState(RS_State::Cull_None);
					bitBlockTransfer_->Blit(postEffectedFrameBuffer_->GetColorMap().GetAddressOf(), 0, 1, toneMapPS_.Get());
				}

				break;
			}
		}
	}

	Render2D();
}

void RenderManager::DrawImGui()
{
	ImGui::Text("Rasterizer State Select");
	ImGui::Text("1 : Cull None");
	ImGui::Text("2 : Cull Back");
	ImGui::Text("3 : Cull Front");
	ImGui::Text("4 : Wireframe");
	ImGui::Text("5 : DebugPrimitive On/Off");

	
	if (ImGui::BeginMenu("Scene Constant"))
	{
		auto& buffer = bufferScene_->data_;
		ImGui::DragFloat3("Light Direction",&buffer.lightDirection_.x,0.01f);
		ImGui::ColorEdit4("Light Color", &buffer.lightColor_.x, ImGuiColorEditFlags_PickerHueWheel);

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
		ImGui::DragFloat("Critical Depth Value", &criticalDepthValue_, 1.0f, 1.0f, 3000.0f);
		ImGui::Checkbox("Colorize Cascaded Layer", reinterpret_cast<bool*>(&bufferEffects_->data_.colorizeCascadedLayer_));
		ImGui::DragFloat("Split Scheme Weight", &cascadedShadowMap_->splitSchemeWeight_, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Critical Depth Value", &criticalDepthValue_, 1.0f, 0.0f, 1000.0f);
		ImGui::DragFloat("Shadow Depth Bias", &bufferEffects_->data_.shadowDepthBias_, 0.000001f, 0.0f, 0.01f, "%.8f");
		ImGui::SliderFloat("Shadow Color", &bufferEffects_->data_.shadowColor_, 0.0f, 1.0f);
		ImGui::SliderFloat("Shadow Filter Radius", &bufferEffects_->data_.shadowFilterRadius_, 0.0f, 64.0f);
		ImGui::SliderInt("Shadow Sample Count", reinterpret_cast<int*>(&bufferEffects_->data_.shadowSampleCount_), 0, 64);

		ImGui::EndMenu();
	}

	//GBuffer���e�N�X�`���\��
	if (ImGui::Begin("G-Buffer"))
	{
		static const char* GBufferNames[] = 
		{
			"Base Color",
			"Emissive",
			"Normal",
			"Parameters",
			"Depth",
		};
		for (int i = GB_BaseColor; i < GB_Max; ++i)
		{
			ImGui::Text(GBufferNames[i]);
			ImGui::Image(gBufferShaderResourceView_[i].Get(), { 256, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		}
		ImGui::End();
	}
}

void RenderManager::ChangeMainCamera(Camera* camera)
{
	for (auto& c : Engine::renderManager_->cameraList_)
	{
		c.lock()->SetIsMainCamera(false);
	}

	camera->SetIsMainCamera(true);
}

void RenderManager::Render2D() const
{
	if (!renderer2DList_.empty())
	{
		//�g�|���W�[�ݒ�
		DXSystem::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		DXSystem::GetDeviceContext()->OMSetBlendState(DXSystem::GetBlendState(BS_State::Alpha),nullptr,0xFFFFFFFF);
		DXSystem::GetDeviceContext()->OMSetDepthStencilState(DXSystem::GetDepthStencilState(DS_State::None_No_Write),0);
		DXSystem::GetDeviceContext()->RSSetState(DXSystem::GetRasterizerState(RS_State::Cull_None));

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
	//�g�|���W�[�ݒ�
	//DXSystem::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

#if _DEBUG
	DebugRSStateSelect();
#endif // _DEBUG

#if ENABLE_DIFFERD_RENDERING
	DXSystem::SetBlendState(BS_State::GBuffer);
#else
	DXSystem::SetBlendState(BS_State::Off);
#endif // ENABLE_DIFFERD_RENDERING

	DXSystem::SetDepthStencilState(DS_State::LEqual);
	//DXSystem::SetRasterizerState(RS_State::Cull_None);
	DXSystem::SetRasterizerState(rasterizerState3D);

#if 0

	DXSystem::GetDeviceContext()->PSSetSamplers(0, 1, samplers_[static_cast<int>(SP_State::Anisotropic)].GetAddressOf());
	DXSystem::GetDeviceContext()->PSSetSamplers(1, 1, samplers_[static_cast<int>(SP_State::Point)].GetAddressOf());
	DXSystem::GetDeviceContext()->PSSetSamplers(2, 1, samplers_[static_cast<int>(SP_State::Linear)].GetAddressOf());
	DXSystem::GetDeviceContext()->PSSetSamplers(3, 1, samplers_[static_cast<int>(SP_State::LinearBorderBlack)].GetAddressOf());
	DXSystem::GetDeviceContext()->PSSetSamplers(4, 1, samplers_[static_cast<int>(SP_State::LinearBorderWhite)].GetAddressOf());
#else
	for (size_t samplerIndex = 0; samplerIndex < _countof(samplers_); ++samplerIndex)
	{
		DXSystem::GetDeviceContext()->PSSetSamplers(static_cast<UINT>(samplerIndex), 1, samplers_[samplerIndex].GetAddressOf());
	}
#endif // 0


	for (auto& r : renderer3DList_)
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

void RenderManager::CheckRenderer()
{
	//3D�A�N�^�[�����m�F
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

	//2D�A�N�^�[�̐����m�F
	//�Q�Ɛ��shared_ptr�������؂ꂵ�Ă���Ȃ�v�f���폜����
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

	DXSystem::GetDeviceContext()->UpdateSubresource(constantBufferScene_.Get(),subresourceIndex, 0, &bufferScene_, 0,0);
	/*const auto hr = DXSystem::GetDeviceContext()->Map(constantBufferScene_.Get(),subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, &bufferScene_, sizeof(constantBufferScene_));
		DXSystem::GetDeviceContext()->Unmap(constantBufferScene_.Get(), subresourceIndex);
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
	DXSystem::GetDeviceContext()->PSSetShaderResources(32, 1, iblShaderResourceView_[0].GetAddressOf());
	DXSystem::GetDeviceContext()->PSSetShaderResources(33, 1, iblShaderResourceView_[1].GetAddressOf());
	DXSystem::GetDeviceContext()->PSSetShaderResources(34, 1, iblShaderResourceView_[2].GetAddressOf());
	DXSystem::GetDeviceContext()->PSSetShaderResources(35, 1, iblShaderResourceView_[3].GetAddressOf());
	DXSystem::GetDeviceContext()->PSSetShaderResources(36, 1, iblShaderResourceView_[4].GetAddressOf());
	DXSystem::GetDeviceContext()->PSSetShaderResources(37, 1, iblShaderResourceView_[5].GetAddressOf());
	DXSystem::GetDeviceContext()->PSSetShaderResources(38, 1, iblShaderResourceView_[6].GetAddressOf());
}

void RenderManager::DebugRSStateSelect()
{
	if (Keyboard::GetKeyDown(DirectX::Keyboard::D1))
	{
		rasterizerState3D = RS_State::Cull_None;
	}
	if (Keyboard::GetKeyDown(DirectX::Keyboard::D2))
	{
		rasterizerState3D = RS_State::Cull_Back;
	}
	if (Keyboard::GetKeyDown(DirectX::Keyboard::D3))
	{
		rasterizerState3D = RS_State::Cull_Front;
	}
	if (Keyboard::GetKeyDown(DirectX::Keyboard::D4))
	{
		rasterizerState3D = RS_State::Wire;
	}
}
