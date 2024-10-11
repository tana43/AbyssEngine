#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "Stage.h"

//�X�e�[�W���Ǘ�����}�l�[�W���[�N���X
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

        //���ݎg�p���Ă�X�e�[�W��Ԃ�
        std::weak_ptr<Stage>& GetActiveStage() { return activeStage_; }

        //�X�e�[�W��ύX����
        //void SetStage(const std::string& name);
        void SetStage(const std::shared_ptr<Stage>& stage);

    private:

        //�X�e�[�W��ǉ�����
        void AddStage(const std::shared_ptr<AbyssEngine::Actor>& stageActor);
        //void 

    private:
        //void ChangeStage();

    private:
        std::weak_ptr<Stage> activeStage_;
        //std::string nextStageName_;

        //�L�[�ƂȂ閼�O�̓X�e�[�W�̃A�N�^�[�����g��
        std::unordered_map<std::string, std::weak_ptr<Stage>> stageMap_;

        friend class Stage;

    };
}

