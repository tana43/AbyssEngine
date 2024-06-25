#include "StageManager.h"
#include "Stage.h"
#include "Actor.h"

void StageManager::SetStage(const std::string& name)
{
    activeStage_ = stageMap_[name];
}

void StageManager::AddStage(const std::shared_ptr<AbyssEngine::Actor>& stageActor)
{
    const auto& com = stageActor->GetComponent<Stage>();
    stageMap_[stageActor->name_] = com;

    //ステージ追加が一度目だったとき、ActiveStageに追加ステージを設定する
    if (stageMap_.size() == 1)
    {
        activeStage_ = com;
    }
}
