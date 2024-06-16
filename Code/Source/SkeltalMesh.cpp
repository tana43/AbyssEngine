#include "SkeletalMesh.h"
#include "Actor.h"
#include "Engine.h"
//#include "FbxMeshData.h"
#include "GltfSkeletalMesh.h"
#include "RenderManager.h"
#include "AssetManager.h"
#include "Animator.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;
using namespace std;

void SkeletalMesh::Initialize(const std::shared_ptr<Actor>& actor)
{
    //マネージャーの登録と初期化
    actor_ = actor;
    transform_ = actor->GetTransform();

	//モデル読み込み
	const auto it = Engine::assetManager_->cacheSkeletalMesh_.find(filePath_);
	if (it != Engine::assetManager_->cacheSkeletalMesh_.end())
	{
		model_ = it->second;
	}
	else
	{
		//一度も読み込まれていないモデルなら新たに読み込み、アセットマネージャーに登録
		model_ = make_shared<GltfSkeletalMesh>(filePath_.c_str());
		Engine::assetManager_->cacheSkeletalMesh_[filePath_] = model_;
	}

	//animatorコンポーネント追加(初期化処理も)
	animator_ = actor->AddComponent<Animator>();
	animator_->LatterInitialize(static_pointer_cast<SkeletalMesh>(shared_from_this()));

	//レンダラーマネージャーに登録
	SetActive(true);
}

void SkeletalMesh::Render()
{
	model_->Draw(DrawPass::Opaque, transform_->GetWorldMatrix(), animator_->GetAnimatedNodes());
	model_->Draw(DrawPass::Transmission, transform_->GetWorldMatrix(), animator_->GetAnimatedNodes());
}

void SkeletalMesh::RenderShadow()
{
	model_->CastShadow(transform_->GetWorldMatrix(), animator_->GetAnimatedNodes());
}

//void SkeletalMesh::AppendAnimation(const std::string& filename)
//{
//	model_->AppendAnimation(filename);
//	animatedNodes_ = model_->nodes_;
//}
//
//void SkeletalMesh::AppendAnimations(const std::vector<std::string>& filenames)
//{
//	model_->AppendAnimations(filenames);
//	animatedNodes_ = model_->nodes_;
//}

void SkeletalMesh::RecalculateFrame()
{
	//アニメーション更新
	animator_->AnimatorUpdate();

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

		/*static int animClip = animationClip_;
		ImGui::SliderInt("Anim Clip", &animClip, 0, model_->animations_.size() - 1);
		animationClip_ = animClip;

		std::string t0 = "Current Anim :";
		std::string t1 = t0 + model_->animations_.at(animationClip_).name_;
		ImGui::Text(t1.c_str());

		ImGui::InputFloat("Time Stamp" ,&timeStamp_);

		ImGui::SliderFloat("Blend Weight", &blendWeight_, 0.0f, 1.0f);*/

		ImGui::TreePop();
	}

	return true;
}

void SkeletalMesh::PlayAnimation(int animIndex,bool loop)
{
	
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
