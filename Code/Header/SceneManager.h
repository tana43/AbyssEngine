#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "Scene.h"
#include <thread>

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
        void TimeUpdate();

        //シーンの追加
        void AddScene(Scene* scene,std::string name);


    public:
        //シーンの変更 ロード済のシーンがあるときは必ずそれを呼ぶこと
        void SetNextScene(std::string name = "");

        //ロードするシーンを設定
        void SetLoadScene(std::string name);

        const bool& GetLoadComplete() const { return loadComplete_; }
        const bool& GetIsChangeSceneFrame() const { return isChangeSceneFrame_; }

    private:
        void DrawImGui();//シーンのImGUi表示
        void DrawImGuiAlways();//常に出すImGUi表示
        void DrawDebug();//シーンのデバッグ表示
        void DrawWorldOutLinerImGui();//ワールド上に生成されるアクターのデバッグ表示

    private:

        //シーンの変更があった場合の更新処理
        void ChangeScene();

        //ロードするシーンがあるならローディング
        static void LoadingScene(SceneManager* scaneManager);

        std::unordered_map<std::string, std::unique_ptr<Scene>> sceneMap_;
        Scene* activeScene_;//現在アクティブなシーン

        Scene* loadScene_; //ロードするシーン
        bool loadComplete_ = false;//ロードが完了しているか

        std::string nextSceneName_; //次のシーン

        //シーン切り替えを１フレーム遅らせるために使う
        bool wait1frame_ = true;//最初のフレームはシーンが空なので遅らせない

        bool isChangeSceneFrame_ = false;

        std::unique_ptr<std::thread> thread_;//スレッド

        friend class Engine;
    };

}

