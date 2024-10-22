#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "Scene.h"
#include <thread>

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
        void TimeUpdate();

        //�V�[���̒ǉ�
        void AddScene(Scene* scene,std::string name);


    public:
        //�V�[���̕ύX ���[�h�ς̃V�[��������Ƃ��͕K��������ĂԂ���
        void SetNextScene(std::string name = "");

        //���[�h����V�[����ݒ�
        void SetLoadScene(std::string name);

        const bool& GetLoadComplete() const { return loadComplete_; }
        const bool& GetIsChangeSceneFrame() const { return isChangeSceneFrame_; }

    private:
        void DrawImGui();//�V�[����ImGUi�\��
        void DrawImGuiAlways();//��ɏo��ImGUi�\��
        void DrawDebug();//�V�[���̃f�o�b�O�\��
        void DrawWorldOutLinerImGui();//���[���h��ɐ��������A�N�^�[�̃f�o�b�O�\��

    private:

        //�V�[���̕ύX���������ꍇ�̍X�V����
        void ChangeScene();

        //���[�h����V�[��������Ȃ烍�[�f�B���O
        static void LoadingScene(SceneManager* scaneManager);

        std::unordered_map<std::string, std::unique_ptr<Scene>> sceneMap_;
        Scene* activeScene_;//���݃A�N�e�B�u�ȃV�[��

        Scene* loadScene_; //���[�h����V�[��
        bool loadComplete_ = false;//���[�h���������Ă��邩

        std::string nextSceneName_; //���̃V�[��

        //�V�[���؂�ւ����P�t���[���x�点�邽�߂Ɏg��
        bool wait1frame_ = true;//�ŏ��̃t���[���̓V�[������Ȃ̂Œx�点�Ȃ�

        bool isChangeSceneFrame_ = false;

        std::unique_ptr<std::thread> thread_;//�X���b�h

        friend class Engine;
    };

}

