#include "SkeletalMesh.h"
#include "Actor.h"
#include "Engine.h"
//#include "FbxMeshData.h"
#include "GltfSkeletalMesh.h"
#include "RenderManager.h"
#include "AssetManager.h"
#include "imgui/imgui.h"

using namespace AbyssEngine;
using namespace std;

void SkeletalMesh::Initialize(const std::shared_ptr<Actor>& actor)
{
    //マネージャーの登録と初期化
    actor_ = actor;
    transform_ = actor->GetTransform();

    //model_ = make_unique<FbxMeshData>(DXSystem::device_.Get(), filePath_.c_str());
	model_ = make_unique<GltfSkeletalMesh>(filePath_.c_str());

	//レンダラーマネージャーに登録
	SetActive(true);
}

void SkeletalMesh::Render()
{
#if 0
	int clipIndex{ 0 };
	int frameIndex{ 0 };
	static float animationTick{ 0 };

	Animation& animation{ model_->animationClips_.at(clipIndex) };
	frameIndex = static_cast<int>(animationTick * animation.samplingRate_);
	if (frameIndex > animation.sequence_.size() - 1)
	{
		frameIndex = 0;
		animationTick = 0;
	}
	else
	{
		animationTick += Time::deltaTime_;
	}
	Animation::Keyframe& keyframe{ animation.sequence_.at(frameIndex) };
#endif
	
	model_->Draw(DrawPass::Opaque, transform_->GetWorldMatrix(), animatedNodes_);
}

void SkeletalMesh::RenderShadow()
{
	model_->CastShadow(transform_->GetWorldMatrix(), animatedNodes_);
}

void SkeletalMesh::AppendAnimation(const std::string& filename)
{
	model_->AppendAnimation(filename);
	animatedNodes_ = model_->nodes_;
}

void SkeletalMesh::AppendAnimations(const std::vector<std::string>& filenames)
{
	model_->AppendAnimations(filenames);
	animatedNodes_ = model_->nodes_;
}

void SkeletalMesh::RecalculateFrame()
{
	transform_->CalcWorldMatrix();//行列更新

//	timeStamp_ += Time::deltaTime_ * animationSpeed_;
//
//#if 0
//	model_->Animate(animationClip_, timeStamp_, animatedNodes_, animationLoop_);
//
//#else
//	//BlendAnimation
//	std::vector<GeometricSubstance::Node> animatedNodes[2];
//	animatedNodes[0] = animatedNodes[1] = model_->nodes_;
//	model_->Animate(animationClip_, timeStamp_, animatedNodes[0]);
//	model_->Animate(animationClip_ + 1, timeStamp_, animatedNodes[1]);
//
//	model_->BlendAnimations(animatedNodes[0], animatedNodes[1], blendWeight_, animatedNodes_);
//
//
//#endif // 0

}

bool SkeletalMesh::DrawImGui()
{
	
	if (ImGui::TreeNode("Skeletal Mesh"))
	{
		ImGui::Checkbox("Enabled",&enabled_);

		static int animClip = animationClip_;
		ImGui::SliderInt("Anim Clip", &animClip, 0, model_->animations_.size() - 1);
		animationClip_ = animClip;

		std::string t0 = "Current Anim :";
		std::string t1 = t0 + model_->animations_.at(animationClip_).name_;
		ImGui::Text(t1.c_str());

		ImGui::InputFloat("Time Stamp" ,&timeStamp_);

		ImGui::SliderFloat("Blend Weight", &blendWeight_, 0.0f, 1.0f);

		ImGui::TreePop();
	}

	return false;
}

void SkeletalMesh::PlayAnimation(int animIndex,bool loop)
{
	_ASSERT_EXPR(animIndex < model_->animations_.size(), u8"指定のアニメーションが見つかりません");

	timeStamp_ = 0.0f;

	animationClip_ = animIndex;
	animationLoop_ = loop;
}

void SkeletalMesh::SetActive(const bool value)
{
	if (value)
	{
		if (!isCalled_)
		{
			Engine::renderManager_->Add(static_pointer_cast<SkeletalMesh>(shared_from_this()));
			isCalled_ = true;
		}
	}
}
//
//void GltfSkeltalMesh::Initialize(const std::shared_ptr<Actor>& actor)
//{
//	//マネージャーの登録と初期化
//	actor_ = actor;
//	transform_ = actor->GetTransform();
//
//	model_ = make_unique<GltfModel>(DXSystem::device_.Get(), filePath_.c_str());
//
//	//レンダラーマネージャーに登録
//	SetActive(true);
//}
//
//void GltfSkeltalMesh::Render()
//{
//	static std::vector<GltfModel::Node> animatedNodes = model_->nodes_;
//	static float time{ 0 };
//	model_->Animate(0, time += Time::deltaTime_, animatedNodes);
//
//	model_->Render(DXSystem::deviceContext_.Get(), transform_->CalcWorldMatrix(), animatedNodes/*UNIT.37*/);
//}
//
//void GltfSkeltalMesh::SetActive(const bool value)
//{
//	if (value)
//	{
//		if (!isCalled_)
//		{
//			Engine::renderManager_->Add(static_pointer_cast<GltfSkeltalMesh>(shared_from_this()));
//			isCalled_ = true;
//		}
//	}
//}
