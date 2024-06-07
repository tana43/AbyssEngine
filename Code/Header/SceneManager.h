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
        ~SceneManager();

        Scene& GetActiveScene(); //現在アクティブなシーンを取得

        bool run = true;   //再生中
        bool pose = false;  //ポーズ中

    private:
        void Exit() const;
        void Update(); //更新

        //シーンの追加
        void AddScene(Scene* scene,std::string name);

        //シーンの変更
        void SetNextScene(std::string name);

        void DrawImGui();//シーンのデバッグ表示
        void DrawWorldOutLinerImGui();//ワールド上に生成されるアクターのデバッグ表示

    private:

        //シーンの変更があった場合の更新処理
        void ChangeScene();

        std::unordered_map<std::string, std::unique_ptr<Scene>> sceneMap_;
        Scene* activeScene_;//現在アクティブなシーン
        std::string nextSceneName_; //次のシーン

        friend class Engine;
    };

}

