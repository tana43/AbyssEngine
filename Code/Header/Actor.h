#pragma once
#include <vector>
#include "Component.h"
#include "Transform.h"
#include "Renderer.h"

namespace AbyssEngine
{
    class Actor final : public std::enable_shared_from_this<Actor>
    {
    public:
        std::string name_;

        template<class T>
        std::shared_ptr<T> GetComponent();//�A�^�b�`����Ă���R���|�[�l���g���������A�Ԃ��i���݂��Ȃ��ꍇnullptr�j
        template<class T>
        std::shared_ptr<T> AddComponent(const char* path = NULL);//�R���|�[�l���g���A�^�b�`����(�����_���[�Ȃǂ̓p�X���K�v)

    public:
        [[nodiscard]] bool GetActive() const { return active_; };   //�A�N�^�[���A�N�e�B�u���ǂ���
        void SetActive(const bool active) { active_ = active; }
        [[nodiscard]] bool GetActiveInHierarchy() const;            //�e�̏�Ԃ��l�����ăA�N�e�B�u��

        [[nodiscard]] std::shared_ptr<Transform>& GetTransform() {return transform_; }
        [[nodiscard]] std::vector<std::shared_ptr<Component>>& GetComponentList() { return componentList_; }

    private:
        void Initialize();  //������
        void Release();     //��n��
        void DrawImGui();

    private:
        std::shared_ptr<Transform> transform_;                  //�A�^�b�`����Ă���Transform
        std::vector<std::shared_ptr<Component>> componentList_; //�A�^�b�`����Ă���R���|�[�l���g�̃��X�g

        bool active_ = true;    //�A�N�e�B�u���
        bool activeOld_ = true; //�ύX���g���K�[�p�L���b�V��

        friend class Scene;
    };

    template<class T>
    inline std::shared_ptr<T> Actor::GetComponent()
    {
        for (std::shared_ptr<Component>& com : componentList_)
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
    inline std::shared_ptr<T> Actor::AddComponent(const char* path)
    {
        std::shared_ptr<T> buff = std::make_shared<T>();

        //�p�X�̓o�^�͕K�v��
        if (path)
        {
            std::dynamic_pointer_cast<Renderer>(buff)->SetFilePath(path);
        }

        //�����A�^�b�`�ł��邩�m�F
        if (std::dynamic_pointer_cast<T>(buff)->CanMultiple())
        {
            std::dynamic_pointer_cast<T>(buff)->Initialize(std::static_pointer_cast<Actor>(shared_from_this()));
            componentList_.emplace_back(buff);
            return buff;
        }

        //���ɃA�^�b�`����Ă��邩�m�F
        bool already_attach = false;
        for (std::shared_ptr<Component> com : componentList_)
        {
            std::shared_ptr<T> _buff = std::dynamic_pointer_cast<T>(com);
            if (_buff != nullptr)
            {
                already_attach = true;
                break;
            }
        }
        if (!already_attach)
        {
            std::dynamic_pointer_cast<T>(buff)->Initialize(std::static_pointer_cast<Actor>(shared_from_this()));
            componentList_.emplace_back(buff);
            return buff;
        }

        return nullptr;
    }
}


