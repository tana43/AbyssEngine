#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "Stage.h"

//ステージを管理するマネージャークラス
namespace AbyssEngine
{

    class StageManager
    {
    public:
        StageManager() {}
        ~StageManager()
        {
            stageMap_.clear();
        }

        //bool RayCast();

        //現在使用してるステージを返す
        std::weak_ptr<Stage>& GetActiveStage() { return activeStage_; }

        //ステージを変更する
        //void SetStage(const std::string& name);
        void SetStage(const std::shared_ptr<Stage>& stage);

    private:

        //ステージを追加する
        void AddStage(const std::shared_ptr<AbyssEngine::Actor>& stageActor);
        //void 

    private:
        //void ChangeStage();

    private:
        std::weak_ptr<Stage> activeStage_;
        //std::string nextStageName_;

        //キーとなる名前はステージのアクター名を使う
        std::unordered_map<std::string, std::weak_ptr<Stage>> stageMap_;

        friend class Stage;

    };
}

