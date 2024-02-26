#pragma once
#include <memory>
#include <vector>
namespace AbyssEngine
{
    class Component;
    class Transform;

    class Actor : public std::enable_shared_from_this<Actor>
    {
    public:
        Actor();
        ~Actor();

        std::shared_ptr<Transform> transform_;                  //�A�^�b�`����Ă���Transform
        std::vector<std::shared_ptr<Component>> componentList_; //�A�^�b�`����Ă���R���|�[�l���g�̃��X�g

        template<class T>
        std::shared_ptr<T> GetComponent();//�A�^�b�`����Ă���R���|�[�l���g���������A�Ԃ��i���݂��Ȃ��ꍇnullptr�j
        template<class T>
        std::shared_ptr<T> AddComponent();//�R���|�[�l���g���A�^�b�`����

        
        [[nodiscard]] bool GetActive() const { return active_; };   //�A�N�^�[���A�N�e�B�u���ǂ���
        void SetActive(const bool active) { active_ = active; }
        [[nodiscard]] bool GetActiveInHierarchy() const;            //�e�̏�Ԃ��l�����ăA�N�e�B�u��

    private:
        void Initialize();  //������
        void Release();     //��n��

    private:
        bool active_ = true;    //�A�N�e�B�u���
        bool activeOld_ = true; //�ύX���g���K�[�p�L���b�V��
    };
}

template<class T>
inline std::shared_ptr<T> AbyssEngine::Actor::GetComponent()
{
    for (std::shared_ptr<Component> com : componentList_)
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
inline std::shared_ptr<T> AbyssEngine::Actor::AddComponent()
{
    std::shared_ptr<T> buff = std::make_shared<T>();

    //�����A�^�b�`�ł��邩�m�F
    if (std::dynamic_pointer_cast<Component>(buff)->CanMultiple())
    {
        std::dynamic_pointer_cast<Component>(buff)->Initialize(std::static_pointer_cast<Actor>(shared_from_this()));
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
        std::dynamic_pointer_cast<Component>(buff)->Initialize(std::static_pointer_cast<Actor>(shared_from_this()));
        componentList_.emplace_back(buff);
        return buff;
    }

    return nullptr;
}
