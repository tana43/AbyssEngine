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
        std::weak_ptr<Actor> Find(const std::string& name_);  //�w�肵�����O�̃A�N�^�[��Ԃ�

        std::string name_;   //�V�[����

        std::shared_ptr<Actor> InstanceActor(const std::string& name_);      //�V�[�����ɃI�u�W�F�N�g�z�u����
        void DestroyActor(const std::shared_ptr<Actor>& actor);             //�A�N�^�[���폜����
        void DestroyComponent(const std::shared_ptr<Component>& component); //�R���|�[�l���g���폜����

    protected:
        virtual void Initialize();  //������
        virtual void Update();      //�X�V
        virtual void Reset();       //�V�[�������Z�b�g
        virtual void Finalize();    //��n��

        virtual void DrawImGui(); //ImGui�f�o�b�O�\��

        std::vector<std::shared_ptr<Actor>> actorList_; //�V�[�����̑S�A�N�^�[

        friend class SceneManager;
    };

}

