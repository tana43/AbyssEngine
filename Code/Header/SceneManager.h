#pragma once
#include <memory>

namespace AbyssEngine
{
    class Scene;

    class SceneManager
    {
    public:
        SceneManager();

        std::unique_ptr<Scene>& GetActiveScene(); //現在アクティブなシーンを取得

        bool run = false;   //再生中
        bool pose = false;  //ポーズ中

    private:
        void Exit() const;
        void Update(); //更新

        std::unique_ptr<Scene> sctiveScene_; //現在アクティブなシーン

    };

}

