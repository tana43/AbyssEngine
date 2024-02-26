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
        std::weak_ptr<Actor> Find(const std::string& name);  //�w�肵�����O�̃A�N�^�[��Ԃ�

        std::string name;//�V�[����

    private:
        std::shared_ptr<Actor> InstanceActor(const std::string& name);      //�V�[�����ɃI�u�W�F�N�g�z�u����
        void DestroyActor(const std::shared_ptr<Actor>& actor);             //�A�N�^�[���폜����
        void DestroyComponent(const std::shared_ptr<Component>& component); //�R���|�[�l���g���폜����

        void Initialize();  //������
        void Update();      //�X�V
        void Reset();       //�V�[�������Z�b�g

        std::vector<std::shared_ptr<Actor>> actorList_; //�V�[�����̑S�A�N�^�[
    };

}

