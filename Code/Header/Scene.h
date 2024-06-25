#pragma once
#include <string>
#include <memory>
#include "Actor.h"
 
namespace AbyssEngine
{
    //�V�[��
    class Scene
    {
    public:
        Scene() {}
        ~Scene() = default;

        std::weak_ptr<Actor> Find(const std::string& name_);  //�w�肵�����O�̃A�N�^�[��Ԃ�

        std::string name_;   //�V�[����

        std::shared_ptr<Actor> InstanceActor(const std::string& name_);      //�V�[�����ɃI�u�W�F�N�g�z�u����
        void DestroyActor(const std::shared_ptr<Actor>& actor);             //�A�N�^�[���폜����
        void DestroyComponent(const std::shared_ptr<Component>& component); //�R���|�[�l���g���폜����
        void DrawWorldOutLinerImGui(); //ImGui�f�o�b�O�\��

    protected:
        virtual void Initialize();  //������
        virtual void Update();      //�X�V
        virtual void Reset();       //�V�[�������Z�b�g
        virtual void Finalize();    //��n��

        virtual void DrawDebug(); //�f�o�b�O�\��
        virtual void DrawImGui(); //ImGui�f�o�b�O�\��
        void ImGuiSaveAllActors();//���ׂẴA�N�^�[�̃g�����X�t�H�[������ۑ�


        std::vector<std::shared_ptr<Actor>> actorList_; //�V�[�����̑S�A�N�^�[

        friend class SceneManager;

    private:
        bool imguiFrag_ = false;
    };

}

