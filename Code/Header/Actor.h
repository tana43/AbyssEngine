#pragma once
#include <vector>
#include "Component.h"
#include "Transform.h"
#include "Renderer.h"
#include "Collider.h"

#include <nlohmann/json.hpp>

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

        //json�ǂݍ��݁A������ΐ���
        nlohmann::json ReadAndCreateJsonFile();
        //Json�ǂݍ��� ���݂��Ȃ��ꍇexist��false��
        nlohmann::json ReadingJsonFile(bool& exist);
        //json��������
        void WritingJsonFile(const nlohmann::json& json);

        static void Destroy(std::shared_ptr<Actor> actor);//�A�N�^�[���폜����

        //�����ɓ������Ă���ꍇ�����ŌĂ΂��
        //�������Ă��鑊��̃R���C�_�[�������Ƃ��ēn�����
        void OnCollision(const std::shared_ptr<Collider>& hitCollider, Collision::IntersectionResult result);

    public:
        [[nodiscard]] std::weak_ptr<Actor> GetParent() const;
        void SetParent(const std::shared_ptr<Actor>& parent);//�e��ݒ�
        void RemoveParent();//�e�q�֌W������


        [[nodiscard]] bool GetActive() const { return active_; };   //�A�N�^�[���A�N�e�B�u���ǂ���
        void SetActive(const bool active) { active_ = active; }
        [[nodiscard]] bool GetActiveInHierarchy() const;            //�e�̏�Ԃ��l�����ăA�N�e�B�u��

        [[nodiscard]] float GetDeltaTime() { return deltaTime_; }
        [[nodiscard]] float GetTimeScale() { return timeScale_; }
        void SetTimeScale(const float scale) { timeScale_ = scale; }

        [[nodiscard]] std::shared_ptr<Transform>& GetTransform() {return transform_; }
        [[nodiscard]] std::vector<std::shared_ptr<Component>>& GetComponentList() { return componentList_; }

        //Imgui����ɕ\�������邩
        bool imguiAlways_ = false;
    private:
        void Initialize();  //������
        void Release();     //��n��
        void DrawImGui();
        void DrawDebug();
        void TimeUpdate(); //�o�ߎ��ԍX�V

    private:
        std::shared_ptr<Transform> transform_;                  //�A�^�b�`����Ă���Transform
        std::vector<std::shared_ptr<Component>> componentList_; //�A�^�b�`����Ă���R���|�[�l���g�̃��X�g

        bool active_ = true;    //�A�N�e�B�u���
        bool activeOld_ = true; //�ύX���g���K�[�p�L���b�V��

        //�e�q�֌W
        std::weak_ptr<Actor> parent_;
        std::vector<std::weak_ptr<Actor>> children_{};

        std::string jsonFilename_;

        float deltaTime_;
        float timeScale_ = 1.0f;

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
            //�R���C�_�[�������̓����_���[���p�������R���|�[�l���g��
            const auto& p = std::dynamic_pointer_cast<Renderer>(buff);
            if (p)p->SetFilePath(path);
            else
            {
                const auto& c = std::dynamic_pointer_cast<Collider>(buff);
                c->SetFilePath(path);
            }
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


