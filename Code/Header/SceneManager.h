#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "Scene.h"

namespace AbyssEngine
{

    class SceneManager
    {
    public:
        SceneManager();

        Scene& GetActiveScene(); //���݃A�N�e�B�u�ȃV�[�����擾

        bool run = true;   //�Đ���
        bool pose = false;  //�|�[�Y��

    private:
        void Exit() const;
        void Update(); //�X�V

        //�V�[���̒ǉ�
        void AddScene(Scene* Scene,std::string name_);

        //�V�[���̕ύX
        void SetNextScene(std::string name_);

        void DrawImGui();

    private:

        //�V�[���̕ύX���������ꍇ�̍X�V����
        void ChangeScene();

        std::unordered_map<std::string, std::unique_ptr<Scene>> sceneMap_;
        Scene* activeScene_;//���݃A�N�e�B�u�ȃV�[��
        std::string nextSceneName_; //���̃V�[��

        friend class Engine;
    };

}

