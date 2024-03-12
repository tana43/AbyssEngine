#pragma once
#include "Actor.h"

namespace AbyssEngine
{
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
        //std::shared_ptr<Component> AddComponent(const std::string& className_); 

    protected:
        virtual void SetActive(bool value) {};//�A�N�e�B�u��Ԃ�؂�ւ���
        Component() = default;

    private:
        //�����R���|�[�l���g�𕡐��A�^�b�`�\��
        virtual bool CanMultiple() { return true; }

        //������
		virtual bool Initialize(const std::shared_ptr<Actor>& actor) { return true; }

        //ImGui�\���p�̋��ʊ֐�
        virtual bool DrawImGui() { return true; }

        friend class Actor;
    };
}

template<class T>
inline std::shared_ptr<T> AbyssEngine::Component::GetComponent()
{
	for (std::shared_ptr<Component>& com : actor_->componentList_)
	{
		std::shared_ptr<T> buff = std::dynamic_pointer_cast<T>(com);
		if (buff != nullptr)
		{
			return buff;
		}
	}
	return nullptr;
}

template<class T>
inline std::shared_ptr<T> AbyssEngine::Component::AddComponent()
{
	std::shared_ptr<T> buff = make_shared<T>();

	//�����A�^�b�`�ł��邩�m�F
	if (dynamic_pointer_cast<Component>(buff)->CanMultiple())
	{
		dynamic_pointer_cast<Component>(buff)->Initialize(actor_);
		actor_->componentList_.emplace_back(buff);
		return buff;
	}

	//���ɃA�^�b�`����Ă��邩�m�F
	bool alreadtAttach_ = false;
	for (std::shared_ptr<Component>& com : actor_->componentList_)
	{
		std::shared_ptr<T> _buff = dynamic_pointer_cast<T>(com);
		if (_buff != nullptr)
		{
			alreadtAttach_ = true;
			break;
		}
	}
	if (!alreadtAttach_)
	{
		dynamic_pointer_cast<Component>(buff)->Initialize(actor_);
		actor_->componentList_.emplace_back(buff);
		return buff;
	}

	return nullptr;
}