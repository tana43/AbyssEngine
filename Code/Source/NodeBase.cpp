#include "JudgmentBase.h"
#include "NodeBase.h"
#include "EnemyBlueSlime.h"
#include "BehaviorData.h"
#include "ActionBase.h"



// デストラクタ
NodeBase::~NodeBase()
{
	delete judgment;
	delete action;
}
// 子ノードゲッター
NodeBase* NodeBase::GetChild(int index)
{
	if (children.size() <= index)//修正
	{
		return nullptr;
	}
	return children.at(index);
}

// 子ノードゲッター(末尾)
NodeBase* NodeBase::GetLastChild()
{
	if (children.size() == 0)
	{
		return nullptr;
	}

	return children.at(children.size() - 1);
}

// 子ノードゲッター(先頭)
NodeBase* NodeBase::GetTopChild()
{
	if (children.size() == 0)
	{
		return nullptr;
	}

	return children.at(0);
}


// ノード検索
NodeBase* NodeBase::SearchNode(std::string searchName)
{
	// 名前が一致
	if (name == searchName)
	{
		return this;
	}
	else {
		// 子ノードで検索
		for (auto itr = children.begin(); itr != children.end(); itr++)
		{
			NodeBase* ret = (*itr)->SearchNode(searchName);

			if (ret != nullptr)
			{
				return ret;
			}
		}
	}

	return nullptr;
}

// ノード推論
NodeBase* NodeBase::Inference(EnemyBlueSlime* enemy, BehaviorData* data)
{
	std::vector<NodeBase*> list;
	NodeBase* result = nullptr;

	// childrenの数だけループを行う。
	for (int i = 0; i < children.size(); i++)
	{
		// children.at(i)->judgmentがnullptrでなければ
		if (children.at(i)->judgment != nullptr)
		{
			// TODO 04_03 children.at(i)->judgment->Judgment()関数を実行し、tureであれば
			// listにchildren.at(i)を追加していく
			if (children.at(i)->judgment->Judgment())
			{
				list.emplace_back(children.at(i));
			}
		}
		else {
			// TODO 04_03 判定クラスがなければ無条件に追加
			list.emplace_back(children.at(i));
		}
	}

	// 選択ルールでノード決め
	switch (selectRule)
	{
		// 優先順位
	case BehaviorTree::SelectRule::Priority:
		result = SelectPriority(&list);
		break;
		// ランダム
	case BehaviorTree::SelectRule::Random:
		result = SelectRandom(&list);
		break;
		// シーケンス
	case BehaviorTree::SelectRule::Sequence:
	case BehaviorTree::SelectRule::SequentialLooping:
		result = SelectSequence(&list, data);
		break;
	}

	if (result != nullptr)
	{
		// 行動があれば終了
		if (result->HasAction() == true)
		{
			return result;
		}
		else {
			// 決まったノードで推論開始
			result = result->Inference(enemy, data);
		}
	}

	return result;
}

// 優先順位でノード選択
NodeBase* NodeBase::SelectPriority(std::vector<NodeBase*>* list)
{
	NodeBase* selectNode = nullptr;
	int priority = INT_MAX;

	// TODO 04_04 listに実行可能なノードが登録されている。
	//listに登録されたノードの中で一番優先順位が高いものを探してselectNodeに格納(数が小さい程優先は高い）
	for (auto& node : *list)
	{
		if (node->priority < priority)
		{
			priority = node->priority;
			selectNode = node;
		}
	}

	return selectNode;
}


// ランダムでノード選択
NodeBase* NodeBase::SelectRandom(std::vector<NodeBase*>* list)
{
	int selectNo = 0;
	// TODO 04_05 listのサイズで乱数を取得してselectNoに格納
	selectNo = rand() % static_cast<int>(list->size());
	
	// listのselectNo番目の実態をリターン
	return (*list).at(selectNo);
}

// シーケンス・シーケンシャルルーピングでノード選択
NodeBase* NodeBase::SelectSequence(std::vector<NodeBase*>* list, BehaviorData* data)
{
	int step = 0;

	// この中間ノードが次に実行すべきstepを取得する
	step = data->GetSequenceStep(name);

	// 中間ノードに登録されているノード数以上の場合、
	if (step >= children.size())
	{
		// TODO 04_06 ルールによってシーケンシャルの処理を切り替える（ルールはthisで参照可能）
		//具体的な手順は以下の通り
		// ①ルールがBehaviorTree::SelectRule::SequentialLoopingのときは最初から実行するため、stepに0を代入
		// ②ルールがBehaviorTree::SelectRule::Sequenceのときは次に実行できるノードがないため、nullptrをリターン
		switch (selectRule)
		{
		case BehaviorTree::SelectRule::Non:
			break;
		case BehaviorTree::SelectRule::Priority:
			break;
		case BehaviorTree::SelectRule::Sequence:
			return nullptr;
			break;
		case BehaviorTree::SelectRule::SequentialLooping:
			step = 0;
			break;
		case BehaviorTree::SelectRule::Random:
			break;
		default:
			break;
		}


	}
	// 実行可能リストに登録されているデータの数だけループを行う
	for (auto itr = list->begin(); itr != list->end(); itr++)
	{
		// 子ノードが実行可能リストに含まれているか
		if (children.at(step)->GetName() == (*itr)->GetName())
		{
			// TODO 04_06 現在の実行ノードをスタックに保存、次に実行するステップの保存を行った後、
			// 現在のステップ番号のノードをリターンしなさい
			//具体的な手順は以下の通り
			// ①スタックにはdata->PushSequenceNode関数を使用する。保存するデータは実行中の中間ノードであるthis。
			// ②次に中間ノードと「次のステップ数」を保存する
			// 　ステップ数の保存にはdata->SetSequenceStep関数を使用する事。
			// 　引数はこの中間ノードの名前(this->name)とステップ数+1です(step + 1)
			// ③ステップ番号目の子ノードを実行ノードとしてリターンする
			data->PushSequenceNode(this);
			data->SetSequenceStep(this->name, step + 1);
			return list->at(step);

		}
	}
	// 指定された中間ノードに実行可能ノードがないのでnullptrをリターンする
	return nullptr;
}

// 判定
bool NodeBase::Judgment(EnemyBlueSlime* enemy)
{
	// TODO 04_07 judgmentがあるか判断。あればメンバ関数Judgment()実行した結果をリターン。（大文字のJと小文字のjに注意）
	if (judgment != nullptr)
	{
		return Judgment(enemy);
	}
	return true;
}

// ノード実行
ActionBase::State NodeBase::Run(EnemyBlueSlime* enemy,float elapsedTime)
{
	// TODO 04_08 actionがあるか判断。あればメンバ関数Run()実行した結果をリターン。
	if (action != nullptr)
	{
		return action->Run(elapsedTime);
	}

	return ActionBase::State::Failed;
}
