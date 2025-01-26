#pragma once
#include "AttackerSystem.h"
#include "ComboSystemInterface.h"
#include "Input.h"

#include <memory>
#include <vector>

template<class T>
class ComboNode : public std::enable_shared_from_this<ComboNode<T>>
{
public:
    ComboNode() {}
    ~ComboNode() = default;

    //実行可否判定
    bool Judgment();

    virtual void Initialize() = 0;
    virtual ComboSystemInterface::State Run() = 0;
    virtual void Finalize() = 0;

    //ノードを探索
    const std::shared_ptr<ComboNode<T>>& SearchNode(std::string searchName);

    //子ノードを追加
    void AddChild(const std::shared_ptr<ComboNode<T>>& node) { children_.emplace_back(node); }

    //次に実行する子ノードを探索
    const std::shared_ptr<ComboNode>& SerachDerivedNode();

    void SetOwner(const std::shared_ptr<T>& owner) { owner_ = owner; }

    void SetName(const std::string& n) { name_ = n; }

    void SetInput(const ComboSystemInterface::InputDirection& i) { input_ = i; }

    void SetActiveJudgment(const bool& active) { activeJudgment_ = active; }

    void TimerReset() { timer_ = 0; }
    void TimerUpdate();

protected:
    std::string name_;

    ComboSystemInterface::InputDirection input_;           //どの入力処理に対応したノードか

    std::vector<std::shared_ptr<ComboNode<T>>> children_;  //子ノード
    std::vector<AbyssEngine::AttackData> attackDatas_;     //攻撃判定データ

    std::weak_ptr<T> owner_;                               //所有者

    float timer_ = 0.0f;                                   //タイマー(※ComboSystemで更新してるから何もしなくていい)

    bool activeJudgment_ = true;                           //ノード実行判定をするか　しない場合はJudgement()がかならずtrueを返すようになる
};

template<class T>
inline bool ComboNode<T>::Judgment()
{
    using namespace AbyssEngine;

    //判定しない場合
    if (!activeJudgment_)return true;

    //攻撃ボタンが押されているか、ルートノードではないか
    if (!Input::GameSupport::GetMeleeAttackButton())return false;

    //スティックの入力値を取得
    auto dir = Input::GameSupport::GetMoveVector();

    //各種入力に応じた判定処理
    //入力方向とそれぞれに応じた方向とで内積を算出し、それを使って判定する
    float dot = 0.0f;
    switch (input_)
    {
    case ComboSystemInterface::InputDirection::Neutral:

        //ニュートラル
        return true;
        break;

    case ComboSystemInterface::InputDirection::Front:

        //前方向
        dot = dir.Dot({ 0,1 });
        break;

    case ComboSystemInterface::InputDirection::Side:

        //横方向
        //左右どちらでも可能にするために内積で出た値を絶対値へ
        dot = fabsf(dir.Dot({ 1,0 }));
        break;

    case ComboSystemInterface::InputDirection::Back:

        //後ろ方向
        dot = fabsf(dir.Dot({ 0,-1 }));

        break;

    default:
        break;
    }

    return dot > 0.5f;
}

template<class T>
inline const std::shared_ptr<ComboNode<T>>& ComboNode<T>::SearchNode(std::string searchName)
{
    //名前一致
    if (name_ == searchName)
    {
        return this->shared_from_this();
    }
    else
    {
        // 子ノードで検索
        for (auto itr = children_.begin(); itr != children_.end(); itr++)
        {
            std::shared_ptr<ComboNode<T>> ret = (*itr)->SearchNode(searchName);

            if (ret != nullptr)
            {
                return ret;
            }
        }
    }

    return nullptr;
}

template<class T>
inline const std::shared_ptr<ComboNode<T>>& ComboNode<T>::SerachDerivedNode()
{
    //子ノードを検索
    for (auto itr = children_.begin(); itr != children_.end(); itr++)
    {
        if ((*itr)->Judgment())
        {
            return(*itr);
        }
    }

    return nullptr;
}

template<class T>
inline void ComboNode<T>::TimerUpdate()
{
    if (const auto& owner = owner_.lock())
    {
        std::shared_ptr<AbyssEngine::Component> com = std::dynamic_pointer_cast<AbyssEngine::Component>(owner);
        timer_ += com->GetActor()->GetDeltaTime();
    }
}


