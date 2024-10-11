#include "StageManager.h"
#include "Stage.h"
#include "Actor.h"

using namespace AbyssEngine;

//void StageManager::SetStage(const std::string& name)
//{
//    if (stageMap_.find(name) == stageMap_.end())
//    {
//        _ASSERT_EXPR(false, L"�w��̃X�e�[�W��������܂���");
//    }
//
//    activeStage_ = stageMap_[name];
//}

void AbyssEngine::StageManager::SetStage(const std::shared_ptr<Stage>& stage)
{
    activeStage_ = stage;

    if (stageMap_.find(stage->GetActor()->name_) == stageMap_.end())
    {
        stageMap_[stage->GetActor()->name_] = stage;
    }
}

void StageManager::AddStage(const std::shared_ptr<AbyssEngine::Actor>& stageActor)
{
    const auto& com = stageActor->GetComponent<Stage>();

    stageMap_[stageActor->name_] = com;

    //�X�e�[�W�ǉ�����x�ڂ������Ƃ��AActiveStage�ɒǉ��X�e�[�W��ݒ肷��
    if (stageMap_.size() == 1)
    {
        activeStage_ = com;
    }
}
