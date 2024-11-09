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

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace AbyssEngine;

void ComputeParticleEmitter::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();

	Engine::renderManager_->Add(std::static_pointer_cast<ComputeParticleEmitter>(shared_from_this()));
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
#if _DEBUG
	if (ImGui::TreeNode("GPU Particle"))
	{
		ImGui::Checkbox("Timeline", &enableTimeline_);
		ImGui::Checkbox("Can Key Emit", &canInputEmit_);
		ImGui::Text("X Key : One Emit");
		ImGui::Text("Z Key : Always Emit");

		if (ImGui::Button("Emit Particle"))
		{
			EmitParticle(debugParam_);
		}
		
		debugParam_.DrawImGui();

		static std::string name;

		ImGui::Text("-------------- Asset Save ---------------");
		static char filename[128] = "";
		ImGui::InputText("Asset Name", filename, ARRAYSIZE(filename));
		if (ImGui::ButtonDoubleChecking("Save", imguiButton_))
		{
			//アセット化
			AssetCreation(debugParam_,filename);

			//文字列リセット
			memset(filename, 0, sizeof(filename));
		}

		if (ImGui::Button("Test Set Parameter"))
		{
			SetEmitParamater(filename);
			debugParam_ = mainParam_;
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
#endif // _DEBUG
}

void AbyssEngine::ComputeParticleEmitter::DrawDebug()
{
#if _DEBUG
	if (canInputEmit_)
	{
		if (Keyboard::GetKeyDown(DirectX::Keyboard::X))
		{
			EmitParticle(debugParam_);
		}

		if (Keyboard::GetKeyState().Z)
		{
			EmitParticle(debugParam_);
		}
	}
#endif // _DEBUG
}

void ComputeParticleEmitter::Update()
{
	

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

void ComputeParticleEmitter::EmitParticle(const EmitParameter& param)
{
	//通常の動作

	//トランスフォーム更新
	const Matrix worldMatrix = transform_->CalcWorldMatrix();

	Vector3 pos;
	if (useTransform_)
	{
		pos = transform_->GetPosition();
	}
	else
	{
		pos = emitPositionNotUseTransform_;
	}

	//各要素の振れ幅を算出
	const Vector3 posAmp = param.positionAmplitude_ / 2.0f;
	const Vector3 veloAmp = param.velocityAmplitude_ / 2.0f;
	const Vector3 accelAmp = param.accelerationAmplitud_ / 2.0f;

	const Vector2 scaleAmp = param.scaleAmplitude_ / 2.0f;
	const Vector2 scaleVeloAmp = param.scaleVelocityAmplitude_ / 2.0f;
	const Vector2 scaleAccelAmp = param.scaleAccelerationAmplitud_ / 2.0f;

	const Vector3 rotAmp = param.scaleAmplitude_ / 2.0f;
	const Vector3 rotVeloAmp = param.scaleVelocityAmplitude_ / 2.0f;
	const Vector3 rotAccelAmp = param.scaleAccelerationAmplitud_ / 2.0f;

	const float lifespanAmp = param.lifespanAmplitude_ / 2.0f;

	const Vector4 colorAmp = param.colorAmplitud_ / 2.0f;

	const float timeScale = actor_->GetDeltaTime() / Time::GetDeltaTime();

	for (int i = 0; i < param.emitNum_; i++)
	{
		//	発生位置
		Vector3 p = { 0,0,0 };
		p.x = pos.x + Math::RandomRange(-posAmp.x, posAmp.x);
		p.y = pos.y + Math::RandomRange(-posAmp.y, posAmp.y);
		p.z = pos.z + Math::RandomRange(-posAmp.z, posAmp.z);

		//	発生方向
		Vector3 v = param.velocity_;
		v.x += Math::RandomRange(-veloAmp.x, veloAmp.x);
		v.y += Math::RandomRange(-veloAmp.y, veloAmp.y);
		v.z += Math::RandomRange(-veloAmp.z, veloAmp.z);
		//	加速度
		Vector3 a = param.acceleration_;
		a.x += Math::RandomRange(-accelAmp.x, accelAmp.x);
		a.y += Math::RandomRange(-accelAmp.y, accelAmp.y);
		a.z += Math::RandomRange(-accelAmp.z, accelAmp.z);

		//　スケール
		Vector3 s = transform_->GetScale() * param.scaleInit_;
		s = s * transform_->GetScaleFactor();
		s.x += Math::RandomRange(-scaleAmp.x, scaleAmp.x);
		s.y += Math::RandomRange(-scaleAmp.y, scaleAmp.y);
		//　スケール速度
		Vector2 sv = param.scaleVelocity_;
		sv.x += Math::RandomRange(-scaleVeloAmp.x, scaleVeloAmp.x);
		sv.y += Math::RandomRange(-scaleVeloAmp.y, scaleVeloAmp.y);
		//　スケール加速度
		Vector2 sa = param.scaleAcceleration_;
		sa.x += Math::RandomRange(-scaleAccelAmp.x, scaleAccelAmp.x);
		sa.y += Math::RandomRange(-scaleAccelAmp.y, scaleAccelAmp.y);

		//　回転
		Vector4 rotation = transform_->GetRotation();
		Vector3 r = { rotation.x,rotation.y,rotation.z };
		r.x += Math::RandomRange(-rotAmp.x, rotAmp.x);
		r.y += Math::RandomRange(-rotAmp.y, rotAmp.y);
		r.z += Math::RandomRange(-rotAmp.z, rotAmp.z);
		//　回転速度
		Vector3 rv = param.rotationVelocity_;
		rv.x += Math::RandomRange(-rotVeloAmp.x, rotVeloAmp.x);
		rv.y += Math::RandomRange(-rotVeloAmp.y, rotVeloAmp.y);
		rv.z += Math::RandomRange(-rotVeloAmp.z, rotVeloAmp.z);
		//　回転加速度
		Vector3 ra = param.rotationAcceleration_;
		ra.x += Math::RandomRange(-rotAccelAmp.x, rotAccelAmp.x);
		ra.y += Math::RandomRange(-rotAccelAmp.y, rotAccelAmp.y);
		ra.y += Math::RandomRange(-rotAccelAmp.z, rotAccelAmp.z);

		ComputeParticleSystem::EmitParticleData data;
		//更新タイプ
		data.parameter_.x = param.texType_;
		data.parameter_.y = param.lifespan_ + Math::RandomRange(-lifespanAmp, lifespanAmp);

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
		data.color_.x = param.color_.x + Math::RandomRange(-colorAmp.x,colorAmp.x);
		data.color_.y = param.color_.y + Math::RandomRange(-colorAmp.y,colorAmp.y);
		data.color_.z = param.color_.z + Math::RandomRange(-colorAmp.z,colorAmp.z);
		data.color_.w = param.color_.w + Math::RandomRange(-colorAmp.w,colorAmp.w);

		data.color_ = data.color_ * param.intensity_;
		Engine::renderManager_->GetParticleSystem()->Emit(data);
	}
}

void AbyssEngine::ComputeParticleEmitter::EmitParticle()
{
	EmitParticle(mainParam_);
}

void ComputeParticleEmitter::SetEmitParamater(std::string filename)
{
	using namespace std;
	string filepath = "./Assets/ParticleEmitParameters/" + filename;
	ifstream ifs(filepath);
	nlohmann::json mJson;
	if (ifs.good())
	{
		//ファイル読み込み
		ifs >> mJson;
	}
	else
	{
		_ASSERT_EXPR(false, L"指定のパーティクルアセットが見つかりません、、、、、、大変お手数なのですが、修正のほどよろしくお願いいたします。");
	}

	//読み込み
	EmitParameter emitParam;
	emitParam.emitNum_           = mJson["emitNum"];
	emitParam.lifespan_          = mJson["lifespan"];
	emitParam.lifespanAmplitude_ = mJson["lifespanAmplitude"];
	emitParam.emitTime_          = mJson["emitTime"];
	emitParam.texType_           = mJson["texType"];

	emitParam.positionAmplitude_         = { mJson["Position"]["amplitude"][0],mJson["Position"]["amplitude"][1],mJson["Position"]["amplitude"][2] };
	emitParam.velocity_                  = { mJson["Position"]["velocity"][0],mJson["Position"]["velocity"][1],mJson["Position"]["velocity"][2] };
	emitParam.velocityAmplitude_         = { mJson["Position"]["vellocityAmplitude"][0],mJson["Position"]["vellocityAmplitude"][1],mJson["Position"]["vellocityAmplitude"][2] };
	emitParam.acceleration_              = { mJson["Position"]["acceleration"][0],mJson["Position"]["acceleration"][1],mJson["Position"]["acceleration"][2] };
	emitParam.accelerationAmplitud_      = { mJson["Position"]["accelerationAmplitude"][0],mJson["Position"]["accelerationAmplitude"][1],mJson["Position"]["accelerationAmplitude"][2] };
	
	emitParam.scaleAmplitude_            = { mJson["Scale"]["amplitude"][0],mJson["Scale"]["amplitude"][1] };
	emitParam.scaleVelocity_             = { mJson["Scale"]["velocity"][0],mJson["Scale"]["velocity"][1] };
	emitParam.scaleVelocityAmplitude_    = { mJson["Scale"]["vellocityAmplitude"][0],mJson["Scale"]["vellocityAmplitude"][1] };
	emitParam.scaleAcceleration_         = { mJson["Scale"]["acceleration"][0],mJson["Scale"]["acceleration"][1] };
	emitParam.scaleAccelerationAmplitud_ = { mJson["Scale"]["accelerationAmplitude"][0],mJson["Scale"]["accelerationAmplitude"][1] };

	emitParam.rotationAmplitude_            = { mJson["Rotation"]["amplitude"][0],mJson["Rotation"]["amplitude"][1],mJson["Rotation"]["amplitude"][2] };
	emitParam.rotationVelocity_             = { mJson["Rotation"]["velocity"][0],mJson["Rotation"]["velocity"][1],mJson["Rotation"]["velocity"][2] };
	emitParam.rotationVelocityAmplitude_    = { mJson["Rotation"]["vellocityAmplitude"][0],mJson["Rotation"]["vellocityAmplitude"][1],mJson["Rotation"]["vellocityAmplitude"][2] };
	emitParam.rotationAcceleration_         = { mJson["Rotation"]["acceleration"][0],mJson["Rotation"]["acceleration"][1],mJson["Rotation"]["acceleration"][2] };
	emitParam.rotationAccelerationAmplitud_ = { mJson["Rotation"]["accelerationAmplitude"][0],mJson["Rotation"]["accelerationAmplitude"][1],mJson["Rotation"]["accelerationAmplitude"][2] };

	emitParam.intensity_	 = mJson["brightness"];
	emitParam.color_		 = { mJson["color"][0],mJson["color"][1],mJson["color"][2],mJson["color"][3] };
	emitParam.colorAmplitud_ = { mJson["colorAmplitude"][0],mJson["colorAmplitude"][1],mJson["colorAmplitude"][2],mJson["colorAmplitude"][3] };

	mainParam_ = emitParam;
}

void AbyssEngine::ComputeParticleEmitter::AssetCreation(const EmitParameter& param, const std::string& filename)
{
	//Jsonファイル作成
	nlohmann::json mJson;

	mJson["emitNum"]           = param.emitNum_;
	mJson["lifespan"]          = param.lifespan_;
	mJson["lifespanAmplitude"] = param.lifespanAmplitude_;
	mJson["emitTime"]          = param.emitTime_;
	mJson["texType"]          = param.texType_;
	mJson["Position"] = 
	{
		{"amplitude",			 {param.positionAmplitude_.x,param.positionAmplitude_.y,param.positionAmplitude_.z}},
		{"velocity",			 {param.velocity_.x,param.velocity_.y,param.velocity_.z}},
		{"vellocityAmplitude",  {param.velocityAmplitude_.x,param.velocityAmplitude_.y,param.velocityAmplitude_.z}},
		{"acceleration",		 {param.acceleration_.x,param.acceleration_.y,param.acceleration_.z}},
		{"accelerationAmplitude",{param.accelerationAmplitud_.x,param.accelerationAmplitud_.y,param.accelerationAmplitud_.z}}
	};
	mJson["Scale"] =
	{
		{"amplitude",			 {param.scaleAmplitude_.x,param.scaleAmplitude_.y}},
		{"velocity",			 {param.scaleVelocity_.x,param.scaleVelocity_.y}},
		{"vellocityAmplitude",  {param.scaleVelocityAmplitude_.x,param.scaleVelocityAmplitude_.y}},
		{"acceleration",		 {param.scaleAcceleration_.x,param.scaleAcceleration_.y}},
		{"accelerationAmplitude",{param.scaleAccelerationAmplitud_.x,param.scaleAccelerationAmplitud_.y}}
	};
	mJson["Rotation"] =
	{
		{"amplitude",			 {param.rotationAmplitude_.x,param.rotationAmplitude_.y,param.rotationAmplitude_.z}},
		{"velocity",			 {param.rotationVelocity_.x,param.rotationVelocity_.y,param.rotationVelocity_.z}},
		{"vellocityAmplitude",  {param.rotationVelocityAmplitude_.x,param.rotationVelocityAmplitude_.y,param.rotationVelocityAmplitude_.z}},
		{"acceleration",		 {param.rotationAcceleration_.x,param.rotationAcceleration_.y,param.rotationAcceleration_.z}},
		{"accelerationAmplitude",{param.rotationAccelerationAmplitud_.x,param.rotationAccelerationAmplitud_.y,param.rotationAccelerationAmplitud_.z}}
	};
	mJson["brightness"]     = param.intensity_;
	mJson["color"]		    = { param.color_.x,param.color_.y,param.color_.z,param.color_.w };
	mJson["colorAmplitude"] = { param.colorAmplitud_.x,param.colorAmplitud_.y,param.colorAmplitud_.z,param.colorAmplitud_.w};

	using namespace std;
	ofstream writingFile;
	string filepath = "./Assets/ParticleEmitParameters/" + filename;
	writingFile.open(filepath, ios::out);
	writingFile << mJson.dump() << endl;
	writingFile.close();
}

ComputeParticleEmitter::EmitParameter AbyssEngine::ComputeParticleEmitter::EmitParameter::operator=(const EmitParameter& param)
{
	emitNum_                      = param.emitNum_;
	lifespan_                     = param.lifespan_;
	lifespanAmplitude_            = param.lifespanAmplitude_;
	emitTime_                      = param.emitTime_;
	texType_                      = param.texType_;
	positionAmplitude_            = param.positionAmplitude_;
	velocity_                     = param.velocity_;
	velocityAmplitude_            = param.velocityAmplitude_;
	acceleration_                 = param.acceleration_;
	accelerationAmplitud_         = param.accelerationAmplitud_;
	scaleInit_					  = param.scaleInit_;
	scaleAmplitude_               = param.scaleAmplitude_;
	scaleVelocity_                = param.scaleVelocity_;
	scaleVelocityAmplitude_       = param.scaleVelocityAmplitude_;
	scaleAcceleration_            = param.scaleAcceleration_;
	scaleAccelerationAmplitud_    = param.scaleAccelerationAmplitud_;
	rotationAmplitude_            = param.rotationAmplitude_;
	rotationVelocity_             = param.rotationVelocity_;
	rotationVelocityAmplitude_    = param.rotationVelocityAmplitude_;
	rotationAcceleration_         = param.rotationAcceleration_;
	rotationAccelerationAmplitud_ = param.rotationAccelerationAmplitud_;
	intensity_                   = param.intensity_;//colorの乗数
	color_                        = param.color_;
	colorAmplitud_                = param.colorAmplitud_;

	return *this;
}

void AbyssEngine::ComputeParticleEmitter::EmitParameter::DrawImGui()
{
	if (ImGui::TreeNode("EmitParamter"))
	{
		ImGui::DragInt("Emit Num", &emitNum_);

		auto spritCount = Engine::renderManager_->GetParticleSystem()->GetTextureSplitCount();
		ImGui::SliderInt("Texture Type", &texType_, 0, spritCount.x * spritCount.y - 1);

		ImGui::DragFloat("Lifespan", &lifespan_, 0.01f, 0.0f);
		ImGui::DragFloat("Lifespan Amplitude", &lifespanAmplitude_, 0.01f, 0.0f);

		ImGui::ColorEdit4("Color", &color_.x, ImGuiColorEditFlags_PickerHueWheel);
		ImGui::DragFloat4("Color Amplitud", &colorAmplitud_.x, 0.001f, 0.0f);

		ImGui::DragFloat("Brightness", &intensity_, 0.01f);

		if (ImGui::TreeNode("Emit Position"))
		{
			ImGui::DragFloat3("Amplitude", &positionAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat3("Velo Init", &velocity_.x, 0.1f);
			ImGui::DragFloat3("Velo Amplitude", &velocityAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat3("Accel", &acceleration_.x, 0.1f);
			ImGui::DragFloat3("Accel Amplitude", &accelerationAmplitud_.x, 0.1f, 0.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Emit Scale"))
		{
			ImGui::DragFloat3("Init", &scaleInit_.x, 0.01f, 0.0f);
			ImGui::DragFloat2("Amplitude", &scaleAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat2("Velo Init", &scaleVelocity_.x, 0.1f);
			ImGui::DragFloat2("Velo Amplitude", &scaleVelocityAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat2("Accel", &scaleAcceleration_.x, 0.1f, 0.0f);
			ImGui::DragFloat2("Accel Amplitude", &scaleAccelerationAmplitud_.x, 0.1f, 0.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Emit Rotation"))
		{
			ImGui::DragFloat3("Amplitude", &rotationAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat3("Velo Init", &rotationVelocity_.x, 0.1f);
			ImGui::DragFloat3("Velo Amplitude", &rotationVelocityAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat3("Accel", &rotationAcceleration_.x, 0.1f, 0.0f);
			ImGui::DragFloat3("Accel Amplitude", &rotationAccelerationAmplitud_.x, 0.1f, 0.0f);

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}
