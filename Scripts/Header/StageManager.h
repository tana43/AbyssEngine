#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "Stage.h"

//ステージを管理するマネージャークラス
//シングルトン
class StageManager
{
private:
    StageManager() {}
    ~StageManager() 
    { 
        stageMap_.clear();
    }

public:
    static StageManager& Instance()
    {
        static StageManager instance;
        return instance;
    }

    //bool RayCast();
    
    //現在使用してるステージを返す
    std::shared_ptr<Stage> GetActiveStage() { return activeStage_; }

    //ステージを変更する
    void SetStage(const std::string& name_);

    //ステージを追加する
    void AddStage(const std::shared_ptr<AbyssEngine::Actor>& stageActor);
    //void 

private:
    //void ChangeStage();

private:
    std::shared_ptr<Stage> activeStage_;
    //std::string nextStageName_;

    //キーとなる名前はステージのアクター名を使う
    std::unordered_map<std::string, std::shared_ptr<Stage>> stageMap_;

};

