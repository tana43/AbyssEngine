// ReSharper disable All
#pragma once
#include <string>
#include <imgui/imgui.h>

#include "Component.h"
#include "JudgmentDerived.h"
#include "ActionDerived.h"
#include <stack>
#include <map>


namespace AbyssEngine
{
	template <class EnemyT>
	class BehaviorData;
	template <class EnemyT>
	class NodeBase;


	// ビヘイビアツリー
	//これがビヘイビアベースの全ての動き
	template <class EnemyT>
	class BehaviorTree : public Component
	{
	public:
		/*
		 * このビヘイビアツリーの組み方やと、シーケンスは中間ノードが全て実行可能ノードに
		 * 登録されてないとうまく推論されない(SelectSequence関数のfor文が原因)
		 * ノードをシーケンスにするときは全ての子ノードを基本的に絶対通るようにする(Failed、実行失敗は大丈夫)
		 * そういう時はPriorityを使おう。うまくやろう。
		 */

		 // 選択ルール
		enum class SelectRule
		{
			Non,				// 無い(末端ノード用)
			Priority,			// 優先順位
			Sequence,			// シーケンス
			SequentialLooping,	// シーケンシャルルーピング
			Random,				// ランダム
		};

		// AIのパターンテーブル
		enum AI_Pattern
		{
			BossEnemy_AI,
			Tutorial_AI,
			CatFish_AI,
			CatFish_TutorialAI,
		};
	public:
		//BehaviorTree() :BaseComponent("BehaviorTree"),root(nullptr), owner(nullptr) {}

		BehaviorTree(EnemyT* enemy, int AI_pattern_) :BaseComponent("BehaviorTree"), root(nullptr), owner(enemy), AI_pattern(AI_pattern_) {}

		~BehaviorTree() { NodeAllClear(root); }


		// 実行ノードを推論する
		NodeBase<EnemyT>* ActiveNodeInference(BehaviorData<EnemyT>* data)
		{
			// データをリセットして開始
			data->Init();
			return root->Inference(data);
		}


		// シーケンスノードから推論開始
		NodeBase<EnemyT>* SequenceBack(NodeBase<EnemyT>* sequenceNode, BehaviorData<EnemyT>* data)
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
			int priority, SelectRule selectRule,
			JudgmentBase<EnemyT>* judgment = nullptr, 
			ActionBase<EnemyT>* action = nullptr)
		{
			// 親の名前が設定されていれば
			if (parentName != "")
			{
				// 設定された親名前を検索
				NodeBase<EnemyT>* parentNode = root->SearchNode(parentName);

				if (parentNode != nullptr)
				{
					// 兄弟ノード
					NodeBase<EnemyT>* sibling = parentNode->GetLastChild();
					NodeBase<EnemyT>* addNode = new NodeBase<EnemyT>(entryName, parentNode, sibling, priority, selectRule, judgment, action, parentNode->GetHirerchyNo() + 1);

					parentNode->AddChild(addNode);
				}
			}
			//親がいなくて
			else {
				// 大元もなければ
				if (root == nullptr)
				{
					// 大元に設定する
					root = new NodeBase<EnemyT>(entryName, nullptr, nullptr, priority, selectRule, judgment, action, 1);
				}
			}
		}



		// 実行
		NodeBase<EnemyT>* Run(NodeBase<EnemyT>* actionNode, BehaviorData<EnemyT>* data, float elapsedTime)
		{
			// ノード実行
			// todo ここをEnemyTにしたら謎エラー出るのはなぜ？
			typename ActionBase<EnemyT>::State state = actionNode->Run(elapsedTime);

			// 正常終了
			if (state == ActionBase<EnemyT>::State::Complete)
			{
				// シーケンスの途中かを判断

				//シーケンスしてる親ノード
				NodeBase<EnemyT>* sequenceNode = data->PopSequenceNode();

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
			else if (state == ActionBase<EnemyT>::State::Failed) {
				return nullptr;
			}

			// 現状維持
			return actionNode;
		}


		// AI_pattern_によって構築することビヘイビアツリーを変える
		void Init() override
		{
			switch (AI_pattern)
			{
			case BossEnemy_AI:
				//BossEnemy_AI_Protocol();
				return;
			case Tutorial_AI:
				//Tutorial_AI_Protocol();
				return;
			case CatFish_AI:
				CatFishAIProtocol();
				return;
			case CatFish_TutorialAI:
				CatFishTutorialAIProtocol();
				return;
			}
			_ASSERT_EXPR(false, L"AIのパターンが不明です");
		}

		void Update() override
		{
			// ここでビヘイビアツリーによる行動遷移

#if 1 こっちで行けるか実験

		// 現在実行するノードがあれば
			if (active_node != nullptr)
			{
				// ビヘイビアツリーからノードを実行。
				active_node = Run(active_node, behaviorData, Dante::Time::Timer::Instance().GetDeltaTime());
			}
			// 現在実行されているノードが無ければ
			if (active_node == nullptr)
			{
				// 次に実行するノードを推論する。
				active_node = ActiveNodeInference(behaviorData);
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
			if (ImGui::TreeNode(name.c_str()))
			{
				//ImGui::Text(active_node->GetName().c_str());
				ImGui::TreePop();
			}
		}

	private:
		// ノード全削除
		void NodeAllClear(NodeBase<EnemyT>* delNode)
		{
			size_t count = delNode->children.size();
			if (count > 0)
			{
				for (NodeBase<EnemyT>* node : delNode->children)
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

	private:
		// ルートノード(ツリーの大元)
		NodeBase<EnemyT>* root;

		// 主にシークエンスの際に使用する。中間ノードとその中間ノードのステップ数などを保持する。
		BehaviorData<EnemyT>* behaviorData = new BehaviorData<EnemyT>;

		// 選択された末端ノード(実際の動作)が入る。
		//常に何か入ってないと全く動作しないことになるのでバグる。
		NodeBase<EnemyT>* active_node = nullptr;

		// todo テンプレート化
		// ビヘイビアツリーの使用者(型はCharacterにすべき？)
		EnemyT* owner;

	private:
		// AIのパターン
		int AI_pattern;

#if 0 ボスAI
		void BossEnemy_AI_Protocol()
		{
			//ビヘイビアツリーの構築
			this->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority);
			{
				// 死亡
				this->AddNode("Root", "death", 0, BehaviorTree::SelectRule::Non, new DeathJudge(owner), new DeathAction(owner));
				// 被ダメリアクション(怯んだあとは瞬間移動)
				this->AddNode("Root", "ReAction", 2, BehaviorTree::SelectRule::Sequence, new FearJudge(owner));
				{
					// 怯み
					this->AddNode("ReAction", "SmallFear", 0, BehaviorTree::SelectRule::Non, nullptr, new FearAction(owner));
					// 遠くにテレポート
					this->AddNode("ReAction", "Teleport(ReAction)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 20.0f));
					// 覚醒
					this->AddNode("ReAction", "Awake", 0, BehaviorTree::SelectRule::Non, new ToAwakeJudge(owner), new AwakeAction(owner));

				}
				// 非戦闘
				this->AddNode("Root", "NonBattle", 1, BehaviorTree::SelectRule::Priority, new IdleJudge(owner));
				{
					this->AddNode("NonBattle", "Idle(NonBattle)", 0, BehaviorTree::SelectRule::Non, nullptr, new IdleAction(owner));
				}
				// 戦闘中
				this->AddNode("Root", "Battle", 3, BehaviorTree::SelectRule::Priority, new BattleJudge(owner));
				{
					// 待機系
					this->AddNode("Battle", "IdleState", 2, BehaviorTree::SelectRule::Random);
					{
						// 歩き
						this->AddNode("IdleState", "Walk", 0, BehaviorTree::SelectRule::Non, nullptr, new WalkAction(owner));

					}
					// 攻撃系
					this->AddNode("Battle", "AttackState", 1, BehaviorTree::SelectRule::Priority, new AttackStateJudge(owner));
					{
						// 近接攻撃
						this->AddNode("AttackState", "NearNormalAttack", 1, BehaviorTree::SelectRule::Non, new NearAttackStateJudge(owner, 4.0f), new NormalAttack(owner));

						// 攻撃をランダムで抽選する
						this->AddNode("AttackState", "RandomAttack", 2, BehaviorTree::SelectRule::Random);
						{
							// 瞬間移動して必殺技
							this->AddNode("RandomAttack", "Teleport->SpecialAttack", 0, BehaviorTree::SelectRule::Sequence, new SpecialAttackJudge(owner));
							{
								// テレポート
								this->AddNode("Teleport->SpecialAttack", "Teleport", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 1.0f));
								// 必殺技
								this->AddNode("Teleport->SpecialAttack", "SpecialAttack", 0, BehaviorTree::SelectRule::Non, nullptr, new SpecialAttackAction(owner));
							}
							// テレポートからの近距離攻撃
							this->AddNode("RandomAttack", "Teleport->NormalAttack(Attack)", 0, BehaviorTree::SelectRule::Sequence);
							{
								// テレポート
								this->AddNode("Teleport->NormalAttack(Attack)", "Teleport(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 1.0f));
								// 近接攻撃
								this->AddNode("Teleport->NormalAttack(Attack)", "NormalAttack(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new NormalAttack(owner));
							}
							// テレポートからの前進攻撃
							this->AddNode("RandomAttack", "Teleport->MoveAttack(Attack)", 0, BehaviorTree::SelectRule::Sequence);
							{
								// テレポート
								this->AddNode("Teleport->MoveAttack(Attack)", "Teleport(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 5.0f));
								// 前進攻撃
								this->AddNode("Teleport->MoveAttack(Attack)", "MoveAttack(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new MoveAttackAction(owner));
							}
							// テレポートからの前進攻撃×2
							this->AddNode("RandomAttack", "Teleport->MoveAttack*2(Attack)", 0, BehaviorTree::SelectRule::Sequence, new AwakeJudge(owner));
							{
								// テレポート
								this->AddNode("Teleport->MoveAttack*2(Attack)", "Teleport(Attack*1)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 5.0f));
								// 前進攻撃
								this->AddNode("Teleport->MoveAttack*2(Attack)", "MoveAttack(Attack*1)", 0, BehaviorTree::SelectRule::Non, nullptr, new MoveAttackAction(owner));
								// テレポート
								this->AddNode("Teleport->MoveAttack*2(Attack)", "Teleport(Attack*2)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 5.0f));
								// 前進攻撃
								this->AddNode("Teleport->MoveAttack*2(Attack)", "MoveAttack(Attack*2)", 0, BehaviorTree::SelectRule::Non, nullptr, new MoveAttackAction(owner));
							}
							// 上空テレポートからのエフェクト攻撃
							this->AddNode("RandomAttack", "HighTeleport->EffectAttack(Attack)", 0, BehaviorTree::SelectRule::Sequence);
							{
								// テレポート
								this->AddNode("HighTeleport->EffectAttack(Attack)", "FirstHighTeleport(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 5.0f, 5.0f));
								// 近接攻撃
								this->AddNode("HighTeleport->EffectAttack(Attack)", "EffectAttack(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new EffectAttack(owner));
								// テレポート
								this->AddNode("HighTeleport->EffectAttack(Attack)", "EndHighTeleport(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 15.0f));
							}
							// 上空テレポートからのエフェクト攻撃×2
							this->AddNode("RandomAttack", "HighTeleport->EffectAttack(Attack)×2", 0, BehaviorTree::SelectRule::Sequence);
							{
								// テレポート
								this->AddNode("HighTeleport->EffectAttack(Attack)×2", "FirstHighTeleport(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 5.0f, 5.0f));
								// エフェクト攻撃
								this->AddNode("HighTeleport->EffectAttack(Attack)×2", "FirstEffectAttack(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new EffectAttack(owner));
								// エフェクト攻撃
								this->AddNode("HighTeleport->EffectAttack(Attack)×2", "SecondEffectAttack(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new EffectAttack(owner));
								// テレポート
								this->AddNode("HighTeleport->EffectAttack(Attack)×2", "EndHighTeleport(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 15.0f));
							}

							// テレポート
							this->AddNode("RandomAttack", "Teleport", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 1.0f));

							// カウンター攻撃
							this->AddNode("RandomAttack", "Counter", 0, BehaviorTree::SelectRule::Non, nullptr, new CounterAttackAction(owner));
							// エフェクト攻撃
							this->AddNode("RandomAttack", "EffectAttack", 0, BehaviorTree::SelectRule::Non, nullptr, new EffectAttack(owner));
						}
					}
					// パリィ系
					this->AddNode("Battle", "ParryCombo", 0, BehaviorTree::SelectRule::Sequence, new ParryJudge(owner));
					{
						// パリィアクション
						this->AddNode("ParryCombo", "Parry", 0, BehaviorTree::SelectRule::Non, nullptr, new ParryAction(owner));

						//パリィ後のアクション
						this->AddNode("ParryCombo", "After(ParryCombo)", 0, BehaviorTree::SelectRule::Random);
						{
							// 遠くにテレポートのみ
							this->AddNode("After(ParryCombo)", "Teleport", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 20.0f));

							// テレポートからの近距離攻撃
							this->AddNode("After(ParryCombo)", "Teleport->NormalAttack(Parry)", 0, BehaviorTree::SelectRule::Sequence);
							{
								// テレポート
								this->AddNode("Teleport->NormalAttack(Parry)", "Teleport(ParryCombo)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 1.0f));
								// 近接攻撃
								this->AddNode("Teleport->NormalAttack(Parry)", "NormalAttack(ParryCombo)", 0, BehaviorTree::SelectRule::Non, nullptr, new NormalAttack(owner));
							}
							// テレポートからの前進攻撃
							this->AddNode("After(ParryCombo)", "Teleport->MoveAttack(Parry)", 0, BehaviorTree::SelectRule::Sequence);
							{
								// テレポート
								this->AddNode("Teleport->MoveAttack(Parry)", "Teleport(ParryCombo)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 5.0f));
								// 近接攻撃
								this->AddNode("Teleport->MoveAttack(Parry)", "MoveAttack(ParryCombo)", 0, BehaviorTree::SelectRule::Non, nullptr, new MoveAttackAction(owner));
							}
						}
					}
				}
			}

		}

#endif

		//void Tutorial_AI_Protocol()
		//{
		//	//ビヘイビアツリーの構築
		//	this->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority);
		//	{
		//		// 怯み
		//		this->AddNode("Root", "SmallFear", 0, BehaviorTree::SelectRule::Non, new FearJudge(owner), new FearAction(owner, FLT_MAX));
		//		// 待機
		//		this->AddNode("Root", "PunchingBag", 1, BehaviorTree::SelectRule::Non, nullptr, new PunchingBagMode(owner));

		//	}
		//}

		void CatFishAIProtocol()
		{
			AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority);
			{
				AddNode("Root", "NonBattle", 0, BehaviorTree::SelectRule::Random);
				{
					// todo 歩行中に気になるものみたいなポイントを置いといて視界にそれが移ったらそっちに行くみたいなことをしたい。
					// どういう風にそのものを見て、どんな感じでそっちに行くのかもパラメータ(性格とか)で個体差を出したい。
					//エージェントアーキテクチャの導入をしてそいつで意思決定を使用
					AddNode("NonBattle", "Wandering", 0, BehaviorTree::SelectRule::Non, nullptr, new WanderingAction(owner));
				}
				//AddNode("Root", "Battle", 1, BehaviorTree::SelectRule::Priority, new BattleJudge(owner));
				{
				    
				}
			}
		}

		void CatFishTutorialAIProtocol()
		{
			//ビヘイビアツリーの構築
			this->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority);
			{
				// 怯み
				this->AddNode("Root", "SmallFear", 0, BehaviorTree::SelectRule::Non, new FearJudge(owner), new CatFishFearAction(owner, FLT_MAX));
				// 待機
				this->AddNode("Root", "PunchingBag", 1, BehaviorTree::SelectRule::Non, nullptr, new CatFishPunchingBagMode(owner));

			}
		}

	};

    

    // ノード
	template <class EnemyT>
	class NodeBase
	{
	public:
		// コンストラクタ
		NodeBase(std::string name, NodeBase* parent, NodeBase* sibling, int priority,
			BehaviorTree<EnemyT>::SelectRule selectRule, JudgmentBase<EnemyT>* judgment, ActionBase<EnemyT>* action, int hierarchyNo) :
			name(name), parent(parent), sibling(sibling), priority(priority),
			selectRule(selectRule), judgment(judgment), action(action), hierarchyNo(hierarchyNo),
			children(NULL)
		{
		}
		// デストラクタ
		~NodeBase()
		{
			delete judgment;
			delete action;
		}


		// 名前ゲッター
		std::string GetName() { return name; }

		// 親ノードゲッター
		NodeBase* GetParent() { return parent; }

		// 子ノードゲッター
		NodeBase* GetChild(int index)
		{
			if (children.size() <= index)
			{
				return nullptr;
			}
			return children.at(index);
		}

		// 子ノードゲッター(末尾)
		NodeBase* GetLastChild()
		{
			if (children.size() == 0)
			{
				return nullptr;
			}

			return children.at(children.size() - 1);
		}

		// 子ノードゲッター(先頭)
		NodeBase* GetTopChild()
		{
			if (children.size() == 0)
			{
				return nullptr;
			}

			return children.at(0);
		}


		// 兄弟ノードゲッター
		NodeBase* GetSibling() { return sibling; }

		// 階層番号ゲッター
		int GetHirerchyNo() { return hierarchyNo; }

		// 優先順位ゲッター
		int GetPriority() { return priority; }

		// 親ノードセッター
		void SetParent(NodeBase* parent) { this->parent = parent; }

		// 子ノード追加
		void AddChild(NodeBase* child) { children.push_back(child); }

		// 兄弟ノードセッター
		void SetSibling(NodeBase* sibling) { this->sibling = sibling; }

		// 行動データを持っているか
		bool HasAction() { return action != nullptr ? true : false; }

		// 実行可否判定
		bool Judgment()
		{
			// judgmentがあるか判断。あればメンバ関数Judgment()実行した結果をリターン。
			if (judgment != nullptr)
			{
				return judgment->Judgment();
			}
			return true;
		}


		// 優先順位選択
		NodeBase* SelectPriority(std::vector<NodeBase*>* list)
		{
			NodeBase* selectNode = nullptr;
			int priority = INT_MAX;

			// 一番優先順位が高いノードを探してselectNodeに格納
			//全ての選択肢の数だけ繰り返す
			for (NodeBase* node : *list)
			{
				//一番優先順位が高いものを探す
				if (node->GetPriority() < priority)
				{
					priority = node->GetPriority();

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
		NodeBase* SelectSequence(std::vector<NodeBase*>& list, BehaviorData<EnemyT>* data)
		{
			// 子ノードのどこまで処理したか
			int step = 0;

			// 指定されている中間ノードのシーケンスがどこまで実行されたか取得する
			step = data->GetSequenceStep(name);

			// 中間ノードに登録されているノード数以上の場合(最後までシーケンスで処理した場合)、
			if (step >= children.size())
			{
				// ルールによって処理を切り替える
				// ルールがBehaviorTree::SelectRule::SequentialLoopingのときは最初から実行するため、stepに0を代入
				if (selectRule == BehaviorTree<EnemyT>::SelectRule::SequentialLooping)
				{
					step = 0;
				}

				// ルールがBehaviorTree::SelectRule::Sequenceのときは次に実行できるノードがないため、nullptrをリターン
				if (selectRule == BehaviorTree<EnemyT>::SelectRule::Sequence)
				{
					return nullptr;
				}
			}

			// シーケンスする実行可能リストが保存されていたら
			if (!data->GetSequenceList().empty())
				//その実行可能リストを使う。
				list = data->GetSequenceList();

			// 実行可能リストに登録されているデータの数だけループを行う
			for (; step < children.size(); step++) {
				for (auto itr = list.begin(); itr != list.end(); itr++)
				{
					// 子ノードが実行可能リストに含まれているか
					if (children.at(step)->GetName() == (*itr)->GetName())
					{
						// 現在の実行ノードの保存、次に実行するステップの保存を行った後、
						// 現在のステップ番号のノードをリターンする
						// (次のフレームでも何のノードを実行してるか判別するため)

						// 実行中の中間ノードを保存。シーケンスしてる親本人ノード。
						data->PushSequenceNode(this);

						// ②また、次に実行する中間ノードとステップ数を保存する
						// 　保存にはdata->SetSequenceStep関数を使用。
						// 　保存データは中間ノードの名前と次のステップ数です(step + 1)
						data->SetSequenceStep(this->name, step + 1);

						// ③ステップ番号目の子ノードを実行ノードとしてリターンする
						return children.at(step);
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



		// ノード推論(次の行動を順に選別する)
		NodeBase* Inference(BehaviorData<EnemyT>* data)
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
				for (int i = 0; i < children.size(); i++)
				{
					// children.at(i)->judgmentがnullptrでなければ
					if (children.at(i)->judgment != nullptr)
					{
						// children.at(i)->judgment->Judgment()関数を実行し、tureであれば
						// listにchildren.at(i)を追加していく
						if (children.at(i)->judgment->Judgment())
						{
							list.emplace_back(children.at(i));
						}
					}
					else {
						// 判定クラスがなければ無条件に追加
						list.emplace_back(children.at(i));
					}
				}
			}

			// 選択ルールでノード決め
			switch (selectRule)
			{
				// 優先順位
			case BehaviorTree<EnemyT>::SelectRule::Priority:
				result = SelectPriority(&list);
				break;
				// ランダム
			case BehaviorTree<EnemyT>::SelectRule::Random:
				result = SelectRandom(&list);
				break;
				// シーケンス
			case BehaviorTree<EnemyT>::SelectRule::Sequence:
			case BehaviorTree<EnemyT>::SelectRule::SequentialLooping:
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
		ActionBase<EnemyT>::State Run(float elapsedTime)
		{
			// actionがあるか判断。あればメンバ関数Run()実行した結果をリターン。
			if (action != nullptr)
			{
				return action->Run(elapsedTime);
			}

			return ActionBase<EnemyT>::State::Failed;
		}




		// 子ノード、末尾のノードではない限り
		//各ノードにもその下の階層のノードを持つ。
		//(一個下の階層だけ、末尾まではない。)
		// todo vectorにしてるってことは同じノードを入れれるから、無理やり確立をいじれるのでは？
		std::vector<NodeBase*>		children;

	protected:

		std::string					name;			// 名前
		BehaviorTree<EnemyT>::SelectRule	selectRule;		// 選択ルール

		// 判定する、判定の内容。継承して判定の内容を自由に作る。
		//nullptrなら判定しないので即選択肢の候補になる
		JudgmentBase<EnemyT>* judgment;	    // 判定クラス

		// 実際の動き、動作の内容。継承して行動の内容を自由に作る。
		//nullptrなら実際の動きはない＝末尾ではないはず
		ActionBase<EnemyT>* action;			// 実行クラス

		unsigned int				priority;		// 優先順位
		NodeBase<EnemyT>* parent;			// 親ノード
		NodeBase<EnemyT>* sibling;		// 兄弟ノード
		int							hierarchyNo;	// 階層番号
	};


	// Behavior保存データ
	template <class EnemyT>
	class BehaviorData
	{
	public:
		// コンストラクタ
		BehaviorData() { Init(); }
		// シーケンスノードのプッシュ
		void PushSequenceNode(NodeBase<EnemyT>* node) { sequenceStack.push(node); }
		// シーケンスノードのポップ
		NodeBase<EnemyT>* PopSequenceNode()
		{
			// 空ならNULL
			if (sequenceStack.empty() != 0)
			{
				return nullptr;
			}

			// シーケンスしてる親ノードを取得
			NodeBase<EnemyT>* node = sequenceStack.top();

			if (node != nullptr)
			{
				// 取り出したデータを削除
				sequenceStack.pop(); //	popは要素を削除する関数
			}

			return node;
		}


		// シーケンスステップのゲッター
		int GetSequenceStep(std::string name)
		{
			if (runSequenceStepMap.count(name) == 0)
			{
				runSequenceStepMap.insert(std::make_pair(name, 0));
			}

			return runSequenceStepMap.at(name);
		}

		// シーケンスステップのセッター
		void SetSequenceStep(std::string name, int step)
		{
			runSequenceStepMap.at(name) = step;
		}

		// 初期化
		void Init()
		{
			runSequenceStepMap.clear();
			while (sequenceStack.size() > 0)
			{
				//スタックトップ(コンテナの末尾側)の要素を削除する
				sequenceStack.pop();
			}
		}


		// シーケンスする実行可能リスト取得
		const std::vector<NodeBase<EnemyT>*>& GetSequenceList() { return sequence_list; }
	private:

		/* シーケンスする実行可能リスト
		 *教材のやり方はシーケンスで次の行動に移行するときに毎回
		 *推論してしまうから、シーケンスとわかったなら一回目に
		 *実行可能リストに入ったアクションを保持してFailedにならない限り
		 *すべて実行する形にする。
		 *(いつかは教材のパターンと使い分けできるようになってもいいかも、
		 *使う機会あるかわからんけど)
		 */
		std::vector<NodeBase<EnemyT>*> sequence_list{};

		// 挿入された側から要素を取り出す(本を積むイメージ)
		// シーケンス実行してる中間ノードをスタック(親本人)
		std::stack<NodeBase<EnemyT>*> sequenceStack;

		// 実行中の中間ノードのステップを記録
		std::map<std::string, int> runSequenceStepMap;
	};


}
