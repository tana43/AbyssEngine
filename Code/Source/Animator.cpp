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

	//��������ɃR���|�[�l���g�̓A�^�b�`�����̂�GetComponent�͋@�\���Ȃ�
	//skeletalMesh_ = actor->GetComponent<SkeletalMesh>();
	//animatedNodes_ = skeletalMesh_.lock()->GetModel()->nodes_;
}

bool Animator::DrawImGui()
{
	if (ImGui::TreeNode("Animator"))
	{
		ImGui::InputFloat("Time Stamp", &timeStamp_);
		ImGui::SliderFloat("Global Anim Speed", &animationSpeed_,0.0f,2.0f);

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
	animatedNodes_ = skeletalMesh->GetModel()->nodes_;

	//�������[�V�������A�j���[�^�[�ɒǉ�
	//���Ȃ炸�O�Ԗڂ̃��[�V�����͑ҋ@�Ɖ���
	animations_.emplace_back(std::make_unique<Animation>(skeletalMesh.get(), "Idle", 0, true));
}

void Animator::AnimatorUpdate()
{
	if (!skeletalMesh_.lock())
	{
		_ASSERT_EXPR("�|�C���^�悪����܂���", false);
		return;
	}
	const auto& model = skeletalMesh_.lock()->GetModel();

	//���ԍX�V
	timeStamp_ += Time::deltaTime_ * animationSpeed_;

	//model->Animate(animationClip_, timeStamp_, animatedNodes_, animationLoop_);
	animations_[animationClip_]->UpdateAnimation(model,timeStamp_);
}

void Animator::PlayAnimation(const size_t& animIndex)
{
	_ASSERT_EXPR(animIndex < animations_.size(), u8"�w��̃A�j���[�V������������܂���");

	timeStamp_ = 0.0f;

	animationClip_ = animIndex;
}

void Animator::PlayAnimation(const std::string& animName)
{
	for (int index = 0; index < animations_.size(); index++)
	{
		if (animations_[index]->name_ == animName)
		{
			timeStamp_ = 0.0f;
			animationClip_ = index;
			return;
		}
	}
	
	_ASSERT_EXPR(false, u8"�w��̃A�j���[�V������������܂���");
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
		u8"�w��̃A�j���[�V�����͑��݂��܂���");

	const auto& model = skeletalMesh_.lock();
	if (!model)return nullptr;

	auto* p = new AnimBlendSpace1D(model.get(), anim);
	animations_.emplace_back(p);
	return p;
}

AnimBlendSpace2D* Animator::AppendAnimation(AnimBlendSpace2D anim)
{
	_ASSERT_EXPR(anim.animIndex_ < skeletalMesh_.lock()->GetModel()->animations_.size(),
		u8"�w��̃A�j���[�V�����͑��݂��܂���");

	const auto& model = skeletalMesh_.lock();
	if (!model)return nullptr;

	auto* p = new AnimBlendSpace2D(model.get(), anim.name_,anim.animIndex_,anim.GetBlendAnims()[0].weight_);
	p->SetBlendAnims(anim.GetBlendAnims());
	animations_.emplace_back(p);
	return p;
}
