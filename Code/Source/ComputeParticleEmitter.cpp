#include "ComputeParticleEmitter.h"
#include "Actor.h"
#include "DxSystem.h"
#include "Texture.h"
#include "Engine.h"
#include "RenderManager.h"
#include "AssetManager.h"

#include "imgui/imgui_neo_sequencer.h"

#include "Input.h"

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
		ImGui::Checkbox("Timeline", &enableTimeline_);
		ImGui::Checkbox("Can Key Emit", &canInputEmit_);
		ImGui::Text("X Key : One Emit");
		ImGui::Text("Z Key : Always Emit");

		if (ImGui::Button("Emit Particle"))
		{
			EmitParticle();
		}
		
		ImGui::DragInt("Emit Num", &emitNum_);

		auto spritCount = Engine::renderManager_->GetParticleSystem()->GetTextureSplitCount();
		ImGui::SliderInt("Texture Type", &texType_,0,spritCount.x * spritCount.y - 1);

		ImGui::DragFloat("Lifespan", &lifespan_, 0.01f,0.0f);
		ImGui::DragFloat("Lifespan Amplitude", &lifespanAmplitude_, 0.01f,0.0f);

		ImGui::ColorEdit4("Color", &color_.x, ImGuiColorEditFlags_PickerHueWheel);
		ImGui::DragFloat4("Color Amplitud", &colorAmplitud_.x,0.001f,0.0f);

		ImGui::DragFloat("Brightness", &brightness_, 0.01f);

		if (ImGui::TreeNode("Emit Position"))
		{
			ImGui::DragFloat3("Amplitude", &positionAmplitude_.x,0.1f, 0.0f);
			ImGui::DragFloat3("Velo Init", &velocity_.x,0.1f);
			ImGui::DragFloat3("Velo Amplitude", &velocityAmplitude_.x,0.1f, 0.0f);
			ImGui::DragFloat3("Accel", &acceleration_.x,0.1f);
			ImGui::DragFloat3("Accel Amplitude", &accelerationAmplitud_.x,0.1f, 0.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Emit Scale"))
		{
			ImGui::DragFloat2("Amplitude", &scaleAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat2("Velo Init", &scaleVelocity_.x, 0.1f);
			ImGui::DragFloat2("Velo Amplitude", &scaleVelocityAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat2("Accel", &scaleAcceleration_.x,0.1f, 0.0f);
			ImGui::DragFloat2("Accel Amplitude", &scaleAccelerationAmplitud_.x, 0.1f, 0.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Emit Rotation"))
		{
			ImGui::DragFloat3("Amplitude", &rotationAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat3("Velo Init", &rotationVelocity_.x, 0.1f);
			ImGui::DragFloat3("Velo Amplitude", &rotationVelocityAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat3("Accel", &rotationAcceleration_.x,0.1f, 0.0f);
			ImGui::DragFloat3("Accel Amplitude", &rotationAccelerationAmplitud_.x, 0.1f, 0.0f);

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	if (enableTimeline_)
	{
		static int32_t currentFrame = 0;
		static int32_t startFrame = -10;
		static int32_t endFrame = 64;
		static bool transformOpen = false;
		std::vector<ImGui::FrameIndexType> keys = { 0, 10, 24 };
		bool doDelete = false;

		ImGui::Begin("Timeline");

		if (ImGui::BeginNeoSequencer("Sequencer", &currentFrame, &startFrame, &endFrame, { 0, 0 },
			ImGuiNeoSequencerFlags_EnableSelection |
			ImGuiNeoSequencerFlags_Selection_EnableDragging |
			ImGuiNeoSequencerFlags_Selection_EnableDeletion))
		{
			if (ImGui::BeginNeoGroup("Transform", &transformOpen))
			{

				if (ImGui::BeginNeoTimelineEx("Position"))
				{
					for (auto&& v : keys)
					{
						ImGui::NeoKeyframe(&v);
						// Per keyframe code here
					}


					if (doDelete)
					{
						uint32_t count = ImGui::GetNeoKeyframeSelectionSize();

						ImGui::FrameIndexType* toRemove = new ImGui::FrameIndexType[count];

						ImGui::GetNeoKeyframeSelection(toRemove);

						//Delete keyframes from your structure
					}
					ImGui::EndNeoTimeLine();
				}
				ImGui::EndNeoGroup();
			}

			ImGui::EndNeoSequencer();
		}
		ImGui::End();
	}
}

void ComputeParticleEmitter::Update()
{
	if (canInputEmit_)
	{
		if (Keyboard::GetKeyDown(DirectX::Keyboard::X)) 
		{
			EmitParticle();
		}

		if (Keyboard::GetKeyState().Z)
		{
			EmitParticle();
		}
	}

	//授業課題の動作
#if 0
	//	スパーク
	if (::GetAsyncKeyState('C') & 0x8000)
	{
		Matrix worldMatrix = transform_->CalcWorldMatrix();

		Vector3 pos = Vector3((rand() % 30 - 15) * 0.1f, rand() % 30 * 0.1f + 1, (rand() % 30 - 15) * 0.1f + 3);
		int max = 100;
		for (int i = 0; i < max; i++)
		{
			Vector3 p;
			p.x = pos.x;
			p.y = pos.y;
			p.z = pos.z;

			Vector3 v = { 0,0,0 };
			v.x = (rand() % 10001 - 5000) * 0.0001f;
			v.y = (rand() % 10001) * 0.0002f + 1.2f;
			v.z = (rand() % 10001 - 5000) * 0.0001f;

			Vector3 f = { 0,-1.2f,0 };
			Vector3 s = { 0.05f,0.05f,0.0f };

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

			data.color_ = color_ * brightness_;

			Engine::renderManager_->GetParticleSystem()->Emit(data);
		}
	}

	//	降雪
	if (::GetAsyncKeyState('X') & 0x8000)
	{
		Vector3 pos = Vector3((rand() % 30 - 15) * 0.1f, rand() % 30 * 0.1f + 1, (rand() % 30 - 15) * 0.1f + 3);
		int max = 100;
		for (int i = 0; i < max; i++)
		{
			//	発生位置
			Vector3 p = { 0,0,0 };
			p.x = pos.x + (rand() % 10001 - 5000) * 0.01f;
			p.y = pos.y;
			p.z = pos.z + (rand() % 10001 - 5000) * 0.01f;
			//	発生方向
			Vector3 v = { 0,0,0 };
			v.y = -(rand() % 10001) * 0.0002f - 0.002f;
			//	力
			Vector3 f = { 0,0,0 };
			f.x = (rand() % 10001) * 0.00001f + 0.1f;
			f.z = (rand() % 10001 - 5000) * 0.00001f;
			//	大きさ
			Vector2 s = { .2f,.2f };
			s = s * transform_->GetScaleFactor();
			p = p * transform_->GetScaleFactor();
			v = v * transform_->GetScaleFactor();
			f = f * transform_->GetScaleFactor();


			ComputeParticleSystem::EmitParticleData data;
			//更新タイプ
			data.parameter_.x = 3;
			data.parameter_.y = 5.0f;

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

			data.rotationVeclocity.z = 1.0f;

			data.color_ = color_ * brightness_;

			Engine::renderManager_->GetParticleSystem()->Emit(data);
		}
	}
#else
#endif // 0
}

void ComputeParticleEmitter::EmitParticle()
{
	//通常の動作

	//トランスフォーム更新
	const Matrix worldMatrix = transform_->CalcWorldMatrix();

	Vector3 pos = transform_->GetPosition();

	//各要素の振れ幅を算出
	const Vector3 posAmp = positionAmplitude_ / 2.0f;
	const Vector3 veloAmp = velocityAmplitude_ / 2.0f;
	const Vector3 accelAmp = accelerationAmplitud_ / 2.0f;

	const Vector2 scaleAmp = scaleAmplitude_ / 2.0f;
	const Vector2 scaleVeloAmp = scaleVelocityAmplitude_ / 2.0f;
	const Vector2 scaleAccelAmp = scaleAccelerationAmplitud_ / 2.0f;

	const Vector3 rotAmp = scaleAmplitude_ / 2.0f;
	const Vector3 rotVeloAmp = scaleVelocityAmplitude_ / 2.0f;
	const Vector3 rotAccelAmp = scaleAccelerationAmplitud_ / 2.0f;

	const float lifespanAmp = lifespanAmplitude_ / 2.0f;

	const Vector4 colorAmp = colorAmplitud_ / 2.0f;

	const float timeScale = actor_->GetDeltaTime() / Time::GetDeltaTime();

	for (int i = 0; i < emitNum_; i++)
	{
		//	発生位置
		Vector3 p = { 0,0,0 };
		p.x = pos.x + Math::RandomRange(-posAmp.x, posAmp.x);
		p.y = pos.y + Math::RandomRange(-posAmp.y, posAmp.y);
		p.z = pos.z + Math::RandomRange(-posAmp.z, posAmp.z);

		//	発生方向
		Vector3 v = velocity_;
		v.x += Math::RandomRange(-veloAmp.x, veloAmp.x);
		v.y += Math::RandomRange(-veloAmp.y, veloAmp.y);
		v.z += Math::RandomRange(-veloAmp.z, veloAmp.z);
		//	加速度
		Vector3 a = acceleration_;
		a.x += Math::RandomRange(-accelAmp.x, accelAmp.x);
		a.y += Math::RandomRange(-accelAmp.y, accelAmp.y);
		a.z += Math::RandomRange(-accelAmp.z, accelAmp.z);

		//　スケール
		Vector3 s = transform_->GetScale();
		s = s * transform_->GetScaleFactor();
		s.x += Math::RandomRange(-scaleAmp.x, scaleAmp.x);
		s.y += Math::RandomRange(-scaleAmp.y, scaleAmp.y);
		//　スケール速度
		Vector2 sv = scaleVelocity_;
		sv.x += Math::RandomRange(-scaleVeloAmp.x, scaleVeloAmp.x);
		sv.y += Math::RandomRange(-scaleVeloAmp.y, scaleVeloAmp.y);
		//　スケール加速度
		Vector2 sa = scaleAcceleration_;
		sa.x += Math::RandomRange(-scaleAccelAmp.x, scaleAccelAmp.x);
		sa.y += Math::RandomRange(-scaleAccelAmp.y, scaleAccelAmp.y);

		//　回転
		Vector4 rotation = transform_->GetRotation();
		Vector3 r = { rotation.x,rotation.y,rotation.z };
		r.x += Math::RandomRange(-rotAmp.x, rotAmp.x);
		r.y += Math::RandomRange(-rotAmp.y, rotAmp.y);
		r.z += Math::RandomRange(-rotAmp.z, rotAmp.z);
		//　回転速度
		Vector3 rv = rotationVelocity_;
		rv.x += Math::RandomRange(-rotVeloAmp.x, rotVeloAmp.x);
		rv.y += Math::RandomRange(-rotVeloAmp.y, rotVeloAmp.y);
		rv.z += Math::RandomRange(-rotVeloAmp.z, rotVeloAmp.z);
		//　回転加速度
		Vector3 ra = rotationAcceleration_;
		ra.x += Math::RandomRange(-rotAccelAmp.x, rotAccelAmp.x);
		ra.y += Math::RandomRange(-rotAccelAmp.y, rotAccelAmp.y);
		ra.y += Math::RandomRange(-rotAccelAmp.z, rotAccelAmp.z);

		ComputeParticleSystem::EmitParticleData data;
		//更新タイプ
		data.parameter_.x = texType_;
		data.parameter_.y = lifespan_ + Math::RandomRange(-lifespanAmp, lifespanAmp);

		//経過時間倍率
		data.parameter_.z = timeScale;

		//発生位置
		data.position_.x = p.x;
		data.position_.y = p.y;
		data.position_.z = p.z;

		//	発生方向
		data.velocity_.x = v.x;
		data.velocity_.y = v.y;
		data.velocity_.z = v.z;

		//	加速力
		data.acceleration_.x = a.x;
		data.acceleration_.y = a.y;
		data.acceleration_.z = a.z;

		//　スケール
		data.scale_.x = s.x;
		data.scale_.y = s.y;
		data.scale_.z = 0.0f;

		//　スケール速度
		data.scaleVelocity_.x = sv.x;
		data.scaleVelocity_.y = sv.y;
		data.scaleVelocity_.z = 0;

		//　スケール加速度
		data.scaleAcceleration_.x = sv.x;
		data.scaleAcceleration_.y = sv.y;
		data.scaleAcceleration_.z = 0;

		//　回転
		data.rotation_.x = r.x;
		data.rotation_.y = r.y;
		data.rotation_.z = r.z;

		//　回転速度
		data.rotationVelocity_.x = rv.x;
		data.rotationVelocity_.y = rv.y;
		data.rotationVelocity_.z = rv.z;

		//　回転加速度
		data.rotationAcceleration_.x = ra.x;
		data.rotationAcceleration_.y = ra.y;
		data.rotationAcceleration_.z = ra.z;

		//　色
		data.color_.x = color_.x + Math::RandomRange(-colorAmp.x,colorAmp.x);
		data.color_.y = color_.y + Math::RandomRange(-colorAmp.y,colorAmp.y);
		data.color_.z = color_.z + Math::RandomRange(-colorAmp.z,colorAmp.z);
		data.color_.w = color_.w + Math::RandomRange(-colorAmp.w,colorAmp.w);

		data.color_ = data.color_ * brightness_;
		Engine::renderManager_->GetParticleSystem()->Emit(data);
	}
}
