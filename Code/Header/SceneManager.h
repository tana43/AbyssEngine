#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "Scene.h"

namespace AbyssEngine
{

    class SceneManager
    {
    public:
        SceneManager();

        Scene& GetActiveScene(); //現在アクティブなシーンを取得

        bool run = true;   //再生中
        bool pose = false;  //ポーズ中

    private:
        void Exit() const;
        void Update(); //更新

        //シーンの追加
        void AddScene(Scene* Scene,std::string name_);

        //シーンの変更
        void SetNextScene(std::string name_);

        void DrawImGui();

    private:

        //シーンの変更があった場合の更新処理
        void ChangeScene();

        std::unordered_map<std::string, std::unique_ptr<Scene>> sceneMap_;
        Scene* activeScene_;//現在アクティブなシーン
        std::string nextSceneName_; //次のシーン

        friend class Engine;
    };

}

