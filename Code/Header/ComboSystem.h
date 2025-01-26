#pragma once

#include "ScriptComponent.h"
#include "ComboNode.h"

#include "imgui/imgui.h"
#include <memory>

template <class T>
class ComboSystem : public AbyssEngine::ScriptComponent
{
public:
    ComboSystem() {}
    ~ComboSystem() {}

    void AddComboNode(
        std::string parentName, 
        std::string entryName, 
        ComboSystemInterface::InputDirection input,
        const std::shared_ptr<ComboNode<T>>& node);

    // 更新処理
    void UpdateBefore()override;

    // ノード実行
    const std::shared_ptr<ComboNode<T>>& Run(const std::shared_ptr<ComboNode<T>>& actionNode);

    void DrawImGui()override;

    //コンボが終了しているか
    bool IsComboFinished();

    //コンボ開始
    void StartCombo();

    //コンボは終わったか
    const bool& GetComboFinished() { return !activeNode_; }

    void SetOwner(const std::shared_ptr<T>& owner) { owner_ = owner; }

private:
    std::shared_ptr<ComboNode<T>> root_;
    std::shared_ptr<ComboNode<T>> activeNode_;
    std::weak_ptr<T> owner_;

    //一度だけ呼ばれる初期化
    bool initOnce_ = false;
};

template<class T>
inline void ComboSystem<T>::AddComboNode(std::string parentName, std::string entryName, ComboSystemInterface::InputDirection input,const std::shared_ptr<ComboNode<T>>& node)
{
    //ノードに名前、判定入力情報、オーナーを設定
    node->SetName(entryName);
    node->SetInput(input);
    node->SetOwner(owner_.lock());//ここでエラーが出るときはおそらくownerが外から設定されていない

    //親の名前が設定されてるか
    if (parentName != "")
    {
        const auto& parentNode = root_->SearchNode(parentName);

        if (parentNode != nullptr)
        {
            parentNode->AddChild(node);

            //ルート直下の初動コンボは無条件で発動させる
            if (parentNode == root_)node->SetActiveJudgment(false);
        }
        else
        {
            _ASSERT_EXPR(false, "親ノードが見つかりません");
        }
    }
    else
    {
        if (root_ == nullptr)
        {
            //されていない場合はこのノードをルートとして追加
            root_ = node;
        }
    }
}

template<class T>
inline void ComboSystem<T>::UpdateBefore()
{
    // コンボが始まっているか
    if (activeNode_ == nullptr)return;

    // 現在のコンボノードを実行
    const auto& node = Run(activeNode_);

    //ノードに変更があった
    if (activeNode_ != node)
    {
        //終了処理
        activeNode_->Finalize();

        activeNode_ = node;

        //nullでなければ初期化処理
        if(activeNode_)activeNode_->Initialize();
    }
}

template<class T>
inline const std::shared_ptr<ComboNode<T>>& ComboSystem<T>::Run(const std::shared_ptr<ComboNode<T>>& actionNode)
{
    //実行ノードがルートノードなら次のノードを検索する
    if (actionNode == root_)
    {
        auto node = actionNode->SerachDerivedNode();

        return node;
    }

    // ノード実行
    ComboSystemInterface::State state = actionNode->Run();

    //タイマー更新
    actionNode->TimerUpdate();

    //正常終了
    if (state == ComboSystemInterface::State::Complete)
    {
        return nullptr;
    }

    //キャンセル可能か
    if (state == ComboSystemInterface::State::Cancellation)
    {
        //次のコンボノードへ派生するか
        auto node = actionNode->SerachDerivedNode();

        if (node)
        {
            //タイマーリセット
            node->TimerReset();

            return node;
        }
    }

    //現状維持
    return actionNode;
}

template<class T>
inline void ComboSystem<T>::DrawImGui()
{
}

template<class T>
inline bool ComboSystem<T>::IsComboFinished()
{
    if (activeNode_)return true;
    else return false;
}

template<class T>
inline void ComboSystem<T>::StartCombo()
{
    //ルートノードからコンボを開始
    activeNode_ = root_;
}

