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

        std::shared_ptr<Transform> transform_;                  //アタッチされているTransform
        std::vector<std::shared_ptr<Component>> componentList_; //アタッチされているコンポーネントのリスト

        template<class T>
        std::shared_ptr<T> GetComponent();//アタッチされているコンポーネントを検索し、返す（存在しない場合nullptr）
        template<class T>
        std::shared_ptr<T> AddComponent();//コンポーネントをアタッチする

        
        [[nodiscard]] bool GetActive() const { return active_; };   //アクターがアクティブかどうか
        void SetActive(const bool active) { active_ = active; }
        [[nodiscard]] bool GetActiveInHierarchy() const;            //親の状態を考慮してアクティブか

    private:
        void Initialize();  //初期化
        void Release();     //後始末

    private:
        bool active_ = true;    //アクティブ状態
        bool activeOld_ = true; //変更時トリガー用キャッシュ
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

    //複数アタッチできるか確認
    if (std::dynamic_pointer_cast<Component>(buff)->CanMultiple())
    {
        std::dynamic_pointer_cast<Component>(buff)->Initialize(std::static_pointer_cast<Actor>(shared_from_this()));
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
        std::dynamic_pointer_cast<Component>(buff)->Initialize(std::static_pointer_cast<Actor>(shared_from_this()));
        componentList_.emplace_back(buff);
        return buff;
    }

    return nullptr;
}
