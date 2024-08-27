#include "Animator.h"
#include "Actor.h"
#include "SkeletalMesh.h"
#include "Engine.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;

void Animator::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();

	//初期化後にコンポーネントはアタッチされるのでGetComponentは機能しない
	//skeletalMesh_ = actor->GetComponent<SkeletalMesh>();
	//animatedNodes_ = skeletalMesh_.lock()->GetModel()->nodes_;
}

bool Animator::DrawImGui()
{
	if (ImGui::TreeNode("Animator"))
	{
		ImGui::InputFloat("Time Stamp", &timeStamp_);
		ImGui::SliderFloat("Global Anim Speed", &animationSpeed_,0.0f,2.0f);
		ImGui::Checkbox("Root Motion", &enableRootMotion_);

		ImGui::Text("----------Current Anim-----------");
		animations_[animationClip_]->DrawImGui(this);

		ImGui::Text("----------Animations----------");
		for (auto& anim : animations_)
		{
			if (animations_[animationClip_]->name_ == anim->name_)continue;
			anim->DrawImGui(this);
		}

		ImGui::TreePop();
	}
	return false;
}

void Animator::LatterInitialize(const std::shared_ptr<SkeletalMesh>& skeletalMesh)
{
	skeletalMesh_ = skeletalMesh;

	//ノードを設定
	animatedNodes_  = nextAnimatedNodes_ = skeletalMesh->GetModel()->nodes_;
	zeroAnimatedNodes_ = animatedNodes_;

	//初期モーションをアニメーターに追加
	//かならず０番目のモーションは待機と仮定
	animations_.emplace_back(std::make_unique<Animation>(skeletalMesh.get(), "Idle", 0, true));
}

void Animator::AnimatorUpdate()
{
	if (!skeletalMesh_.lock())
	{
		_ASSERT_EXPR("ポインタ先がありません", false);
		return;
	}
	const auto& model = skeletalMesh_.lock()->GetModel();

	//再生終了フラグリセット
	isFinished_ = false;

	//model->Animate(animationClip_, timeStamp_, animatedNodes_, animationLoop_);
	
	//現在モーションの遷移ブレンド中か
	if (isTransitioningBlendAnim_)
	{
		//モーションの遷移中
		transitionTimer_ -= Time::deltaTime_;
		if (transitionTimer_ < 0)transitionTimer_ = 0;

		animatedNodes_ = animations_[animationClip_]->UpdateAnimation(model);
		nextAnimatedNodes_ = animations_[nextAnimationClip_]->UpdateAnimation(model);

		float weight = transitionTimer_ / transitionTimeRequired_;
		model->BlendAnimations(nextAnimatedNodes_, animatedNodes_, weight, animatedNodes_);

		//遷移が完了していないか
		if (weight <= 0)
		{
			//完了処理
			isTransitioningBlendAnim_ = false;
			animationClip_ = nextAnimationClip_;
			nextAnimationClip_ = -1;
		}
	}
	else
	{
		//通常の更新と再生終了フラグ更新
		animatedNodes_ = animations_[animationClip_]->UpdateAnimation(model,&isFinished_);
	}

	//ルートモーション
	if (enableRootMotion_)
	{
		GltfSkeletalMesh::Node& node = animatedNodes_.at(rootJointIndex_);

		//初期姿勢を設定
		if (!isSetZeroAnimatedNodes_)
		{
			model->Animate(0, 0, zeroAnimatedNodes_);
			isSetZeroAnimatedNodes_ = true;
		}

		if (animations_[animationClip_]->GetTimeStamp() == 0)
		{
			//アニメーションタイマーが０のときに前回姿勢をリセットする
			previousPosition_ = { node.globalTransform_._41, node.globalTransform_._42, node.globalTransform_._43 }; // global space
		}

		//グローバル空間でのモーションによる移動値を算出
		Vector3 position = { node.globalTransform_._41,node.globalTransform_._42,node.globalTransform_._43 };
		Vector3 displacement = position - previousPosition_;

		//グローバル → ワールド空間へ変換
		auto& t = transform_;
		const Vector3 scale = t->GetScale() * t->GetScaleFactor();
		const Vector4& r = t->GetRotation();
		Vector3 euler = { r.x,r.y,r.z };
		Matrix S = Matrix::CreateScale(scale.x, scale.y, scale.z);
		Matrix R = Matrix::CreateFromQuaternion(Quaternion::Euler(euler));
		displacement = DirectX::XMVector3TransformNormal(displacement,S * R);

		//移動させる
		Vector3 translation = transform_->GetPosition();
		translation = translation + displacement;
		transform_->SetPosition(translation);

		//ルートノードの変換行列のオフセットを初期姿勢の値に設定
		node.globalTransform_._41 = zeroAnimatedNodes_.at(rootJointIndex_).globalTransform_._41;
		node.globalTransform_._42 = zeroAnimatedNodes_.at(rootJointIndex_).globalTransform_._42;
		node.globalTransform_._43 = zeroAnimatedNodes_.at(rootJointIndex_).globalTransform_._43;

		// 子ノードを再更新
		std::function<void(int, int)> traverse = [&](int parentIndex, int nodeIndex)
		{
			GltfSkeletalMesh::Node& node = animatedNodes_.at(nodeIndex);
			if (parentIndex > -1)
			{
				DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale_.x, node.scale_.y, node.scale_.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotation_.x, node.rotation_.y, node.rotation_.z, node.rotation_.w));
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translation_.x, node.translation_.y, node.translation_.z);
				DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T * DirectX::XMLoadFloat4x4(&animatedNodes_.at(parentIndex).globalTransform_));
			}
			for (int child_index : node.children_)
			{
				traverse(nodeIndex, child_index);
			}
		};
		traverse(-1, rootJointIndex_);

		previousPosition_ = position;
	}

	//時間更新
	timeStamp_ += Time::deltaTime_ * animationSpeed_;
	
}

void Animator::PlayAnimation(const size_t& animIndex, float transTime)
{
	_ASSERT_EXPR(animIndex < animations_.size(), u8"指定のアニメーションが見つかりません");

	PlayAnimationCommon(animIndex,transTime);
}

void Animator::PlayAnimation(const std::string& animName, float transTime)
{
	for (size_t index = 0; index < animations_.size(); index++)
	{
		if (animations_[index]->name_ == animName)
		{
			PlayAnimationCommon(index,transTime);
			return;
		}
	}
	
	_ASSERT_EXPR(false, u8"指定のアニメーションが見つかりません");
}

void Animator::PlayAnimationCommon(const size_t& animIndex,float transTime)
{
	//変更後が今と同じアニメーションなら処理しない
	if (animationClip_ == animIndex)return;

	if (transTime > 0)
	{
		timeStamp_ = 0.0f;
		nextAnimationClip_ = animIndex;
		isTransitioningBlendAnim_ = true;
		transitionTimeRequired_ = transTime;
		transitionTimer_ = transTime;
	}
	else
	{
		timeStamp_ = 0.0f;
		animationClip_ = animIndex;
		nextAnimationClip_ = -1;
		isTransitioningBlendAnim_ = false;
		transitionTimeRequired_ = 0;
		transitionTimer_ = 0;
	}

	animations_[animIndex]->Initialize();
}

//void Animator::ReloadAnimation()
//{
//
//}

void Animator::AppendAnimation(const std::string& filename, const std::string& motionName)
{
	const auto& model = skeletalMesh_.lock();
	_ASSERT_EXPR(model, "");
	
	model->GetModel()->AppendAnimation(filename);
	animatedNodes_ = model->GetModel()->nodes_;
	
	animations_.emplace_back(std::make_unique<Animation>(model.get(), motionName, animations_.size()));
}

void Animator::AppendAnimations(const std::vector<std::string>& filenames, const std::vector<std::string>& motionNames)
{
	const auto& model = skeletalMesh_.lock();
	if (!model)return;

	model->GetModel()->AppendAnimations(filenames);
	for (int i = 0; i < filenames.size(); i++)
	{
		animations_.emplace_back(std::make_unique<Animation>(model.get(), motionNames[i], animations_.size()));
	}
}

AnimBlendSpace1D* Animator::AppendAnimation(AnimBlendSpace1D anim)
{
	_ASSERT_EXPR(anim.animIndex_ < skeletalMesh_.lock()->GetModel()->animations_.size(),
		u8"指定のアニメーションは存在しません");

	const auto& model = skeletalMesh_.lock();
	if (!model)return nullptr;

	auto* p = new AnimBlendSpace1D(model.get(), anim);
	animations_.emplace_back(p);
	return p;
}

AnimBlendSpace2D* Animator::AppendAnimation(AnimBlendSpace2D anim)
{
	_ASSERT_EXPR(anim.animIndex_ < skeletalMesh_.lock()->GetModel()->animations_.size(),
		u8"指定のアニメーションは存在しません");

	const auto& model = skeletalMesh_.lock();
	if (!model)return nullptr;

	auto* p = new AnimBlendSpace2D(model.get(), anim.name_,anim.animIndex_,anim.GetBlendAnims()[0].weight_);
	p->SetBlendAnims(anim.GetBlendAnims());
	animations_.emplace_back(p);
	return p;
}

AnimBlendSpaceFlyMove* AbyssEngine::Animator::AppendAnimation(AnimBlendSpaceFlyMove anim)
{
	const auto& model = skeletalMesh_.lock();
	if (!model)return nullptr;

	auto* p = new AnimBlendSpaceFlyMove(model.get(), anim.name_,anim.GetBlendSpace2D(),anim.GetBlendSpace1D());
	animations_.emplace_back(p);
	return p;
}

std::vector<Animation*> AbyssEngine::Animator::GetAnimations()
{
	std::vector<Animation*> anims;
	for (const auto& a : animations_)
	{
		anims.emplace_back(a.get());
	}
	return anims;
};
