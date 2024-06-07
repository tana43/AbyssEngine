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
        ~SceneManager();

        Scene& GetActiveScene(); //���݃A�N�e�B�u�ȃV�[�����擾

        bool run = true;   //�Đ���
        bool pose = false;  //�|�[�Y��

    private:
        void Exit() const;
        void Update(); //�X�V

        //�V�[���̒ǉ�
        void AddScene(Scene* scene,std::string name);

        //�V�[���̕ύX
        void SetNextScene(std::string name);

        void DrawImGui();//�V�[���̃f�o�b�O�\��
        void DrawWorldOutLinerImGui();//���[���h��ɐ��������A�N�^�[�̃f�o�b�O�\��

    private:

        //�V�[���̕ύX���������ꍇ�̍X�V����
        void ChangeScene();

        std::unordered_map<std::string, std::unique_ptr<Scene>> sceneMap_;
        Scene* activeScene_;//���݃A�N�e�B�u�ȃV�[��
        std::string nextSceneName_; //���̃V�[��

        friend class Engine;
    };

}

