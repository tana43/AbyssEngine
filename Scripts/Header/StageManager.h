#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "Stage.h"

//�X�e�[�W���Ǘ�����}�l�[�W���[�N���X
//�V���O���g��
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
    
    //���ݎg�p���Ă�X�e�[�W��Ԃ�
    std::shared_ptr<Stage> GetActiveStage() { return activeStage_; }

    //�X�e�[�W��ύX����
    void SetStage(const std::string& name_);

    //�X�e�[�W��ǉ�����
    void AddStage(const std::shared_ptr<AbyssEngine::Actor>& stageActor);
    //void 

private:
    //void ChangeStage();

private:
    std::shared_ptr<Stage> activeStage_;
    //std::string nextStageName_;

    //�L�[�ƂȂ閼�O�̓X�e�[�W�̃A�N�^�[�����g��
    std::unordered_map<std::string, std::shared_ptr<Stage>> stageMap_;

};

