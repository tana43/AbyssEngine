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
        std::shared_ptr<T> GetComponent();//アタッチされているコンポーネントを検索し、返す（存在しない場合nullptr）
        template<class T>
        std::shared_ptr<T> AddComponent(const char* path = NULL);//コンポーネントをアタッチする(レンダラーなどはパスが必要)

    public:
        [[nodiscard]] bool GetActive() const { return active_; };   //アクターがアクティブかどうか
        void SetActive(const bool active) { active_ = active; }
        [[nodiscard]] bool GetActiveInHierarchy() const;            //親の状態を考慮してアクティブか

        [[nodiscard]] std::shared_ptr<Transform>& GetTransform() {return transform_; }
        [[nodiscard]] std::vector<std::shared_ptr<Component>>& GetComponentList() { return componentList_; }

    private:
        void Initialize();  //初期化
        void Release();     //後始末
        void DrawImGui();

    private:
        std::shared_ptr<Transform> transform_;                  //アタッチされているTransform
        std::vector<std::shared_ptr<Component>> componentList_; //アタッチされているコンポーネントのリスト

        bool active_ = true;    //アクティブ状態
        bool activeOld_ = true; //変更時トリガー用キャッシュ

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

        //パスの登録は必要か
        if (path)
        {
            std::dynamic_pointer_cast<Renderer>(buff)->SetFilePath(path);
        }

        //複数アタッチできるか確認
        if (std::dynamic_pointer_cast<T>(buff)->CanMultiple())
        {
            std::dynamic_pointer_cast<T>(buff)->Initialize(std::static_pointer_cast<Actor>(shared_from_this()));
            componentList_.emplace_back(buff);
            return buff;
        }

        //既にアタッチされているか確認
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


