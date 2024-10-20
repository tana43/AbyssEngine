#include "ComputeParticleEmitter.h"
#include "Actor.h"
#include "DxSystem.h"
#include "Texture.h"
#include "Engine.h"
#include "RenderManager.h"
#include "AssetManager.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;

void ComputeParticleEmitter::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();

	Engine::renderManager_->Add(std::static_pointer_cast<ComputeParticleEmitter>(shared_from_this()));

	texture_ = Texture::Load("./Assets/Effects/Texture/Particle04_bokashistrong_soft.png");
}

void ComputeParticleEmitter::Render()
{
	
}

void ComputeParticleEmitter::RecalculateFrame()
{
	Update();
}

void ComputeParticleEmitter::DrawImGui()
{
	if (ImGui::TreeNode("GPU Particle"))
	{
		ImGui::ColorPicker4("Color", &color_.x, ImGuiColorEditFlags_PickerHueWheel);

		ImGui::TreePop();
	}
}

void ComputeParticleEmitter::Update()
{
	//	スパーク
	if (::GetAsyncKeyState('C') & 0x8000)
	{
		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3((rand() % 30 - 15) * 0.1f, rand() % 30 * 0.1f + 1, (rand() % 30 - 15) * 0.1f + 3);
		int max = 100;
		for (int i = 0; i < max; i++)
		{
			DirectX::XMFLOAT3 p;
			p.x = pos.x;
			p.y = pos.y;
			p.z = pos.z;

			DirectX::XMFLOAT3 v = { 0,0,0 };
			v.x = (rand() % 10001 - 5000) * 0.0001f;
			v.y = (rand() % 10001) * 0.0002f + 1.2f;
			v.z = (rand() % 10001 - 5000) * 0.0001f;

			DirectX::XMFLOAT3 f = { 0,-1.2f,0 };
			DirectX::XMFLOAT2 s = { 0.05f,0.05f };

			ComputeParticleSystem::EmitParticleData data;
			//更新タイプ
			data.parameter_.x = 2;
			data.parameter_.y = 3.0f;

			//発生位置
			data.position_.x = p.x;
			data.position_.y = p.y;
			data.position_.z = p.z;

			//	発生方向
			data.velocity_.x = v.x;
			data.velocity_.y = v.y;
			data.velocity_.z = v.z;

			//	加速力
			data.acceleration_.x = f.x;
			data.acceleration_.y = f.y;
			data.acceleration_.z = f.z;

			//	大きさ
			data.scale_.x = s.x;
			data.scale_.y = s.y;
			data.scale_.z = 0.0f;

			data.color_ = color_;

			Engine::renderManager_->GetParticleSystem()->Emit(data);
		}
	}
}
