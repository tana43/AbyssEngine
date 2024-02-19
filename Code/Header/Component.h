#pragma once
#include <memory>
#include <string>

namespace AbyssEngine
{
    class Actor;
    class Transform;

    class Component
    {
    public:
        std::shared_ptr<Actor> actor_; //�A�^�b�`���Ă���Actor
        std::shared_ptr<Transform> transform_;//�A�^�b�`���Ă���Actor��Transform

        //Actor�ɃA�^�b�`���ꂽ�R���|�[�l���g���擾�i���݂��Ȃ��ꍇnullptr�j
        template<class T>
        std::shared_ptr<T> GetComponent(); 

        //Actor�ɃR���|�[�l���g���A�^�b�`����i�e���v���[�g�Łj
        template<class T>
        std::shared_ptr<T> AddComponent();                                      

        //Actor�ɃR���|�[�l���g���A�^�b�`����i�N���X���Łj
        std::shared_ptr<Component> AddComponent(const std::string& className_); 

    protected:
        virtual void SetActive(bool value) {};//�A�N�e�B�u��Ԃ�؂�ւ���
        Component() = default;

    private:
        //�����R���|�[�l���g�𕡐��A�^�b�`�\��
        virtual bool CanMultiple() { return true; }

        //������
        virtual bool Initialize(const std::shared_ptr<Actor>& actor) {}

        //ImGui�\���p�̋��ʊ֐�
        virtual bool DrawImGui() { return true; }

        friend class Actor;
    };
}