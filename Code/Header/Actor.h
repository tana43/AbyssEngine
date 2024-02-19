#pragma once
#include <memory>
#include <vector>
#include "Component.h"

namespace AbyssEngine
{
    class Actor : public std::enable_shared_from_this<Actor>
    {
    public:
        Actor();
        ~Actor();

        std::shared_ptr<Transform> transform_;                  //アタッチされているTransform
        std::vector<std::shared_ptr<Component>> componentList_; //アタッチされているコンポーネントのリスト

    private:
        void Initialize();  //初期化
        void Release();     //後始末

    private:
        bool active_ = true;    //アクティブ状態
        bool activeOld_ = true; //変更時トリガー用キャッシュ
    };
}

