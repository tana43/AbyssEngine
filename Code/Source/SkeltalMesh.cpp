#include "SkeltalMesh.h"
#include "Actor.h"
#include "Engine.h"
#include "FbxMeshData.h"
#include "RenderManager.h"
#include "AssetManager.h"
#include "GltfModel.h"

using namespace AbyssEngine;
using namespace std;

void SkeltalMesh::Initialize(const std::shared_ptr<Actor>& actor)
{
    //マネージャーの登録と初期化
    actor_ = actor;
    transform_ = actor->GetTransform();

    model_ = make_unique<FbxMeshData>(DXSystem::device_.Get(), filePath_.c_str());

	//レンダラーマネージャーに登録
	SetActive(true);
}

void SkeltalMesh::Render()
{
	if (model_->animationClips_.size() > 0)
	{
#if 1
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
		model_->Render(DXSystem::deviceContext_.Get(), transform_->CalcWorldMatrix(), color_, &keyframe);
	}
	else
	{
		model_->Render(DXSystem::deviceContext_.Get(), transform_->CalcWorldMatrix(), color_, nullptr);
	}
}

void SkeltalMesh::SetActive(const bool value)
{
	if (value)
	{
		if (!isCalled_)
		{
			Engine::renderManager_->Add(static_pointer_cast<SkeltalMesh>(shared_from_this()));
			isCalled_ = true;
		}
	}
}

void AbyssEngine::GltfSkeltalMesh::Initialize(const std::shared_ptr<Actor>& actor)
{
	//マネージャーの登録と初期化
	actor_ = actor;
	transform_ = actor->GetTransform();

	model_ = make_unique<GltfModel>(DXSystem::device_.Get(), filePath_.c_str());

	//レンダラーマネージャーに登録
	SetActive(true);
}
