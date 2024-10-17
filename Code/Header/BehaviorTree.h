// ReSharper disable All
#pragma once
#include <string>
#include <imgui/imgui.h>

#include "ScriptComponent.h"
#include "JudgmentDerived.h"
#include "ActionDerived.h"
#include "Engine.h"
#include <stack>
#include <map>


namespace AbyssEngine
{
	template <class T>
	class BehaviorData;
	template <class T>
	class NodeBase;


	// ビヘイビアツリー
	//これがビヘイビアベースの全ての動き
	template <class T>
	class BehaviorTree : public ScriptComponent
	{
		/*
		* このビヘイビアツリーの組み方やと、シーケンスは中間ノードが全て実行可能ノードに
		* 登録されてないとうまく推論されない(SelectSequence関数のfor文が原因)
		* ノードをシーケンスにするときは全ての子ノードを基本的に絶対通るようにする(Failed、実行失敗は大丈夫)
		* そういう時はPriorityを使おう。うまくやろう。
		*/
	public:
		
		enum class SelectRule
		{
			Non,				// 無い(末端ノード用)
			Priority,			// 優先順位
			Sequence,			// シーケンス
			SequentialLooping,	// シーケンシャルルーピング
			Random,				// ランダム
		};

	public:
		BehaviorTree() : root_(nullptr) {}

		~BehaviorTree() { NodeAllClear(root_); }

		// AI_pattern_によって構築することビヘイビアツリーを変える
		void Initialize(const std::shared_ptr<Actor>& actor) override
		{
			ScriptComponent::Initialize(actor);
		}

		//通常更新よりも先に更新させるためにUpdateBeforeを使う
		void UpdateBefore() override
		{
			// ここでビヘイビアツリーによる行動遷移

#if 1 こっちで行けるか実験

		// 現在実行するノードがあれば
			if (activeNode_ != nullptr)
			{
				// ビヘイビアツリーからノードを実行。
				activeNode_ = Run(activeNode_, behaviorData_, actor_->GetDeltaTime());
			}
			// 現在実行されているノードが無ければ
			if (activeNode_ == nullptr)
			{
				// 次に実行するノードを推論する。
				activeNode_ = ActiveNodeInference(behaviorData_);
			}

# else かつてのやり方
		// 現在実行されているノードが無ければ
			if (active_node == nullptr)
			{
				// 次に実行するノードを推論する。
				active_node = ActiveNodeInference(behaviorData);
			}
			// 現在実行するノードがあれば
			if (active_node != nullptr)
			{
				// ビヘイビアツリーからノードを実行。
				active_node = Run(active_node, behaviorData, Nero::Time::Timer::Instance().GetDeltaTime());
			}

#endif

		}


		void DrawImGui() override
		{
			if (ImGui::TreeNode("BehaviorTree"))
			{
				if (active_)
				{
					ImGui::Text(activeNode_->GetName().c_str());
				}

				ImGui::Checkbox("Active", &active_);

				ImGui::TreePop();
			}
		}


		// 実行ノードを推論する
		NodeBase<T>* ActiveNodeInference(BehaviorData<T>* data)
		{
			// データをリセットして開始
			data->Init();
			return root_->Inference(data);
		}


		// シーケンスノードから推論開始
		NodeBase<T>* SequenceBack(NodeBase<T>* sequenceNode, BehaviorData<T>* data)
		{
			return sequenceNode->Inference(data);
		}


		// ノード追加
		// 引数の順に
		// 親名前、ルートノードの場合はどこからも繋がってきていないので空文字を入れる。
		// 自分の名前、重複したらバグる。
		// 優先順位、値が低いものが優先される。
		// 選択アルゴリズム、各ルールはenum class SelectRule で定義されてる。
		// 判定するか、登録する際に new で欲しい判定クラスを生成する。
		// 行動があるか。ルートノードや中間ノードの場合は行動が無いのでnullptr を指定する。判定同様 new で生成。
		void AddNode(std::string parentName, std::string entryName,
			int priority_, SelectRule selectRule_,
			JudgmentBase<T>* judgment_ = nullptr, 
			ActionBase<T>* action_ = nullptr)
		{
			// 親の名前が設定されていれば
			if (parentName != "")
			{
				// 設定された親名前を検索
				NodeBase<T>* parentNode = root_->SearchNode(parentName);

				if (parentNode != nullptr)
				{
					// 兄弟ノード
					NodeBase<T>* sibling_ = parentNode->GetLastChild();
					NodeBase<T>* addNode = new NodeBase<T>(entryName, parentNode, sibling_, priority_, selectRule_, judgment_, action_, parentNode->GetHirerchyNo() + 1);

					parentNode->AddChild(addNode);
				}
			}
			//親がいなくて
			else {
				// 大元もなければ
				if (root_ == nullptr)
				{
					// 大元に設定する
					root_ = new NodeBase<T>(entryName, nullptr, nullptr, priority_, selectRule_, judgment_, action_, 1);
				}
			}
		}



		// 実行
		NodeBase<T>* Run(NodeBase<T>* actionNode, BehaviorData<T>* data, float elapsedTime)
		{
			// ノード実行
			typename ActionBase<T>::State state = actionNode->Run(elapsedTime);

			// 正常終了
			if (state == ActionBase<T>::State::Complete)
			{
				// シーケンスの途中かを判断

				//シーケンスしてる親ノード
				NodeBase<T>* sequenceNode = data->PopSequenceNode();

				// 途中じゃないなら終了
				if (sequenceNode == nullptr)
				{
					// todo ここでnullptrを返すのが割と都合悪い
					return nullptr;
				}
				else {
					// 途中ならそこから始める
					return SequenceBack(sequenceNode, data);
				}
			}
			// 失敗は終了
			else if (state == ActionBase<T>::State::Failed) {
				return nullptr;
			}

			// 現状維持
			return actionNode;
		}

	private:
		// ノード全削除
		void NodeAllClear(NodeBase<T>* delNode)
		{
			size_t count = delNode->children_.size();
			if (count > 0)
			{
				for (NodeBase<T>* node : delNode->children_)
				{
					NodeAllClear(node);
				}
				delete delNode;
			}
			else
			{
				delete delNode;
			}
		}

	public:
		void SetOwner(const std::shared_ptr<T>& owner) { owner_ = owner; }

	private:
		// ルートノード(ツリーの大元)
		NodeBase<T>* root_;

		// 主にシークエンスの際に使用する。中間ノードとその中間ノードのステップ数などを保持する。
		BehaviorData<T>* behaviorData_ = new BehaviorData<T>;

		// 選択された末端ノード(実際の動作)が入る。
		//常に何か入ってないと全く動作しないことになるのでバグる。
		NodeBase<T>* activeNode_ = nullptr;

		// todo テンプレート化
		// ビヘイビアツリーの使用者(型はCharacterにすべき？)
		std::weak_ptr<T> owner_;

	};


    // ノード
	template <class T>
	class NodeBase
	{
	public:
		// コンストラクタ
		NodeBase(std::string name_, NodeBase* parent_, NodeBase* sibling_, int priority_,
			BehaviorTree<T>::SelectRule selectRule_, JudgmentBase<T>* judgment_, ActionBase<T>* action_, int hierarchyNo_) :
			name_(name_), parent_(parent_), sibling_(sibling_), priority_(priority_),
			selectRule_(selectRule_), judgment_(judgment_), action_(action_), hierarchyNo_(hierarchyNo_),
			children_(NULL)
		{
		}
		// デストラクタ
		~NodeBase()
		{
			delete judgment_;
			delete action_;
		}


		// 名前ゲッター
		std::string GetName() { return name_; }

		// 親ノードゲッター
		NodeBase* GetParent() { return parent_; }

		// 子ノードゲッター
		NodeBase* GetChild(int index)
		{
			if (children_.size() <= index)
			{
				return nullptr;
			}
			return children_.at(index);
		}

		// 子ノードゲッター(末尾)
		NodeBase* GetLastChild()
		{
			if (children_.size() == 0)
			{
				return nullptr;
			}

			return children_.at(children_.size() - 1);
		}

		// 子ノードゲッター(先頭)
		NodeBase* GetTopChild()
		{
			if (children_.size() == 0)
			{
				return nullptr;
			}

			return children_.at(0);
		}


		// 兄弟ノードゲッター
		NodeBase* GetSibling() { return sibling_; }

		// 階層番号ゲッター
		int GetHirerchyNo() { return hierarchyNo_; }

		// 優先順位ゲッター
		int GetPriority() { return priority_; }

		// 親ノードセッター
		void SetParent(NodeBase* parent_) { this->parent_ = parent_; }

		// 子ノード追加
		void AddChild(NodeBase* child) { children_.push_back(child); }

		// 兄弟ノードセッター
		void SetSibling(NodeBase* sibling_) { this->sibling_ = sibling_; }

		// 行動データを持っているか
		bool HasAction() { return action_ != nullptr ? true : false; }

		// 実行可否判定
		bool Judgment()
		{
			// judgmentがあるか判断。あればメンバ関数Judgment()実行した結果をリターン。
			if (judgment_ != nullptr)
			{
				return judgment_->Judgment();
			}
			return true;
		}


		// 優先順位選択
		NodeBase* SelectPriority(std::vector<NodeBase*>* list)
		{
			NodeBase* selectNode = nullptr;
			int priority_ = INT_MAX;

			// 一番優先順位が高いノードを探してselectNodeに格納
			//全ての選択肢の数だけ繰り返す
			for (NodeBase* node : *list)
			{
				//一番優先順位が高いものを探す
				if (node->GetPriority() < priority_)
				{
					priority_ = node->GetPriority();

					// 最終的に一番優先順位の高いものを選択する。
					selectNode = node;
				}
			}
			
			return selectNode;
		}

		// ランダム選択
		NodeBase* SelectRandom(std::vector<NodeBase*>* list)
		{
			int selectNo = 0;
			// listのサイズで乱数を取得してselectNoに格納
			selectNo = rand() % list->size();

			// listのselectNo番目の実態をリターン
			return (*list).at(selectNo);
		}

		// シーケンス選択
		NodeBase* SelectSequence(std::vector<NodeBase*>& list, BehaviorData<T>* data)
		{
			// 子ノードのどこまで処理したか
			int step = 0;

			// 指定されている中間ノードのシーケンスがどこまで実行されたか取得する
			step = data->GetSequenceStep(name_);

			// 中間ノードに登録されているノード数以上の場合(最後までシーケンスで処理した場合)、
			if (step >= children_.size())
			{
				// ルールによって処理を切り替える
				// ルールがBehaviorTree::SelectRule::SequentialLoopingのときは最初から実行するため、stepに0を代入
				if (selectRule_ == BehaviorTree<T>::SelectRule::SequentialLooping)
				{
					step = 0;
				}

				// ルールがBehaviorTree::SelectRule::Sequenceのときは次に実行できるノードがないため、nullptrをリターン
				if (selectRule_ == BehaviorTree<T>::SelectRule::Sequence)
				{
					return nullptr;
				}
			}

			// シーケンスする実行可能リストが保存されていたら
			if (!data->GetSequenceList().empty())
				//その実行可能リストを使う。
				list = data->GetSequenceList();

			// 実行可能リストに登録されているデータの数だけループを行う
			for (; step < children_.size(); step++) {
				for (auto itr = list.begin(); itr != list.end(); itr++)
				{
					// 子ノードが実行可能リストに含まれているか
					if (children_.at(step)->GetName() == (*itr)->GetName())
					{
						// 現在の実行ノードの保存、次に実行するステップの保存を行った後、
						// 現在のステップ番号のノードをリターンする
						// (次のフレームでも何のノードを実行してるか判別するため)

						// 実行中の中間ノードを保存。シーケンスしてる親本人ノード。
						data->PushSequenceNode(this);

						// ②また、次に実行する中間ノードとステップ数を保存する
						// 　保存にはdata->SetSequenceStep関数を使用。
						// 　保存データは中間ノードの名前と次のステップ数です(step + 1)
						data->SetSequenceStep(this->name_, step + 1);

						// ③ステップ番号目の子ノードを実行ノードとしてリターンする
						return children_.at(step);
					}
				}
			}

			// 指定された中間ノードに実行可能ノードがないのでnullptrをリターンする
			return nullptr;
		}



		// ノード検索(自分と自分の全ての子の名前を比較して検索する)
		NodeBase* SearchNode(std::string searchName)
		{
			// 名前が一致
			if (name_ == searchName)
			{
				return this;
			}
			else {
				// 子ノードで検索
				for (auto itr = children_.begin(); itr != children_.end(); itr++)
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



		// ノード推論(次の行動を順に選別する)
		NodeBase* Inference(BehaviorData<T>* data)
		{
			// 次の行動のリスト、ここにできる選択肢の行動が入る
			std::vector<NodeBase*> list;
			// 決定されたノードが入る
			NodeBase* result = nullptr;

			// シーケンスする実行可能リストが空なら(シーケンスしてない、またはまだ登録されてないってこと)
			if (data->GetSequenceList().empty())
			{
				// 次の行動の選択肢を絞ってる

				// childrenの数だけループを行う。
				for (int i = 0; i < children_.size(); i++)
				{
					// children.at(i)->judgmentがnullptrでなければ
					if (children_.at(i)->judgment_ != nullptr)
					{
						// children.at(i)->judgment->Judgment()関数を実行し、tureであれば
						// listにchildren.at(i)を追加していく
						if (children_.at(i)->judgment_->Judgment())
						{
							list.emplace_back(children_.at(i));
						}
					}
					else {
						// 判定クラスがなければ無条件に追加
						list.emplace_back(children_.at(i));
					}
				}
			}

			// 選択ルールでノード決め
			switch (selectRule_)
			{
				// 優先順位
			case BehaviorTree<T>::SelectRule::Priority:
				result = SelectPriority(&list);
				break;
				// ランダム
			case BehaviorTree<T>::SelectRule::Random:
				result = SelectRandom(&list);
				break;
				// シーケンス
			case BehaviorTree<T>::SelectRule::Sequence:
			case BehaviorTree<T>::SelectRule::SequentialLooping:
				result = SelectSequence(list, data);
				break;
			}

			// ここでアクションがあれば推論が終了
			// 違う場合はさらに一個下のノードから選択肢を探す
			// アクションが見つかるまでやる。アクションがなかったらバグるので注意。
			if (result != nullptr)
			{
				// 行動があれば終了
				if (result->HasAction() == true)
				{
					return result;
				}
				else {
					// 決まったノードでまた推論開始
					result = result->Inference(data);
				}
			}


			return result;
		}


		// 実行(選別された行動を実行、updateみたいなやつ)
		ActionBase<T>::State Run(float elapsedTime)
		{
			// actionがあるか判断。あればメンバ関数Run()実行した結果をリターン。
			if (action_ != nullptr)
			{
				return action_->Run(elapsedTime);
			}

			return ActionBase<T>::State::Failed;
		}




		// 子ノード、末尾のノードではない限り
		//各ノードにもその下の階層のノードを持つ。
		//(一個下の階層だけ、末尾まではない。)
		// todo vectorにしてるってことは同じノードを入れれるから、無理やり確立をいじれるのでは？
		std::vector<NodeBase*>	children_;

	protected:

		std::string	name_;			// 名前
		BehaviorTree<T>::SelectRule selectRule_;		// 選択ルール

		// 判定する、判定の内容。継承して判定の内容を自由に作る。
		//nullptrなら判定しないので即選択肢の候補になる
		JudgmentBase<T>* judgment_;	// 判定クラス

		// 実際の動き、動作の内容。継承して行動の内容を自由に作る。
		//nullptrなら実際の動きはない＝末尾ではないはず
		ActionBase<T>* action_;		// 実行クラス

		unsigned int priority_;			// 優先順位
		NodeBase<T>* parent_;		// 親ノード
		NodeBase<T>* sibling_;		// 兄弟ノード
		int	hierarchyNo_;				// 階層番号
	};


	// Behavior保存データ
	template <class T>
	class BehaviorData
	{
	public:
		// コンストラクタ
		BehaviorData() { Init(); }
		// シーケンスノードのプッシュ
		void PushSequenceNode(NodeBase<T>* node) { sequenceStack_.push(node); }
		// シーケンスノードのポップ
		NodeBase<T>* PopSequenceNode()
		{
			// 空ならNULL
			if (sequenceStack_.empty() != 0)
			{
				return nullptr;
			}

			// シーケンスしてる親ノードを取得
			NodeBase<T>* node = sequenceStack_.top();

			if (node != nullptr)
			{
				// 取り出したデータを削除
				sequenceStack_.pop(); //	popは要素を削除する関数
			}

			return node;
		}


		// シーケンスステップのゲッター
		int GetSequenceStep(std::string name_)
		{
			if (runSequenceStepMap_.count(name_) == 0)
			{
				runSequenceStepMap_.insert(std::make_pair(name_, 0));
			}

			return runSequenceStepMap_.at(name_);
		}

		// シーケンスステップのセッター
		void SetSequenceStep(std::string name_, int step)
		{
			runSequenceStepMap_.at(name_) = step;
		}

		// 初期化
		void Init()
		{
			runSequenceStepMap_.clear();
			while (sequenceStack_.size() > 0)
			{
				//スタックトップ(コンテナの末尾側)の要素を削除する
				sequenceStack_.pop();
			}
		}


		// シーケンスする実行可能リスト取得
		const std::vector<NodeBase<T>*>& GetSequenceList() { return sequenceList_; }
	private:

		/* シーケンスする実行可能リスト
		 *教材のやり方はシーケンスで次の行動に移行するときに毎回
		 *推論してしまうから、シーケンスとわかったなら一回目に
		 *実行可能リストに入ったアクションを保持してFailedにならない限り
		 *すべて実行する形にする。
		 *(いつかは教材のパターンと使い分けできるようになってもいいかも、
		 *使う機会あるかわからんけど)
		 */
		std::vector<NodeBase<T>*> sequenceList_{};

		// 挿入された側から要素を取り出す(本を積むイメージ)
		// シーケンス実行してる中間ノードをスタック(親本人)
		std::stack<NodeBase<T>*> sequenceStack_;

		// 実行中の中間ノードのステップを記録
		std::map<std::string, int> runSequenceStepMap_;
	};


}
