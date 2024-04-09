#include "SkeltalMesh.h"
#include "Actor.h"
#include "Engine.h"

using namespace AbyssEngine;
using namespace std;

void SkeltalMesh::Initialize(const std::shared_ptr<Actor>& actor)
{
    //マネージャーの登録と初期化
    actor_ = actor;
    transform_ = actor->GetTransfrom();

    model_ = make_unique<MeshData>(DXSystem::device_.Get(), filePath_);
}

void AbyssEngine::SkeltalMesh::Render()
{
	if (model_->animationClips_.size() > 0)
	{
#if 1
		int clip_index{ 0 };
		int frame_index{ 0 };
		static float animation_tick{ 0 };

		Animation& animation{ model_->animationClips_.at(clip_index) };
		frame_index = static_cast<int>(animation_tick * animation.samplingRate_);
		if (frame_index > animation.sequence_.size() - 1)
		{
			frame_index = 0;
			animation_tick = 0;
		}
		else
		{
			animation_tick += Time::deltaTime_;
		}
		Animation::Keyframe& keyframe{ animation.sequence_.at(frame_index) };
#endif
		model_->Render(DXSystem::deviceContext_.Get(), transform_->CalcWorldMatrix(), color_, &keyframe);
	}
	else
	{
		model_->Render(DXSystem::deviceContext_.Get(), transform_->CalcWorldMatrix(), color_, nullptr);
	}
}
