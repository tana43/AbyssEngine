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


	// �r�w�C�r�A�c���[
	//���ꂪ�r�w�C�r�A�x�[�X�̑S�Ă̓���
	template <class EnemyT>
	class BehaviorTree : public Component
	{
	public:
		/*
		 * ���̃r�w�C�r�A�c���[�̑g�ݕ���ƁA�V�[�P���X�͒��ԃm�[�h���S�Ď��s�\�m�[�h��
		 * �o�^����ĂȂ��Ƃ��܂����_����Ȃ�(SelectSequence�֐���for��������)
		 * �m�[�h���V�[�P���X�ɂ���Ƃ��͑S�Ă̎q�m�[�h����{�I�ɐ�Βʂ�悤�ɂ���(Failed�A���s���s�͑��v)
		 * ������������Priority���g�����B���܂���낤�B
		 */

		 // �I�����[��
		enum class SelectRule
		{
			Non,				// ����(���[�m�[�h�p)
			Priority,			// �D�揇��
			Sequence,			// �V�[�P���X
			SequentialLooping,	// �V�[�P���V�������[�s���O
			Random,				// �����_��
		};

		// AI�̃p�^�[���e�[�u��
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


		// ���s�m�[�h�𐄘_����
		NodeBase<EnemyT>* ActiveNodeInference(BehaviorData<EnemyT>* data)
		{
			// �f�[�^�����Z�b�g���ĊJ�n
			data->Init();
			return root->Inference(data);
		}


		// �V�[�P���X�m�[�h���琄�_�J�n
		NodeBase<EnemyT>* SequenceBack(NodeBase<EnemyT>* sequenceNode, BehaviorData<EnemyT>* data)
		{
			return sequenceNode->Inference(data);
		}


		// �m�[�h�ǉ�
		// �����̏���
		// �e���O�A���[�g�m�[�h�̏ꍇ�͂ǂ�������q�����Ă��Ă��Ȃ��̂ŋ󕶎�������B
		// �����̖��O�A�d��������o�O��B
		// �D�揇�ʁA�l���Ⴂ���̂��D�悳���B
		// �I���A���S���Y���A�e���[����enum class SelectRule �Œ�`����Ă�B
		// ���肷�邩�A�o�^����ۂ� new �ŗ~��������N���X�𐶐�����B
		// �s�������邩�B���[�g�m�[�h�⒆�ԃm�[�h�̏ꍇ�͍s���������̂�nullptr ���w�肷��B���蓯�l new �Ő����B
		void AddNode(std::string parentName, std::string entryName,
			int priority, SelectRule selectRule,
			JudgmentBase<EnemyT>* judgment = nullptr, 
			ActionBase<EnemyT>* action = nullptr)
		{
			// �e�̖��O���ݒ肳��Ă����
			if (parentName != "")
			{
				// �ݒ肳�ꂽ�e���O������
				NodeBase<EnemyT>* parentNode = root->SearchNode(parentName);

				if (parentNode != nullptr)
				{
					// �Z��m�[�h
					NodeBase<EnemyT>* sibling = parentNode->GetLastChild();
					NodeBase<EnemyT>* addNode = new NodeBase<EnemyT>(entryName, parentNode, sibling, priority, selectRule, judgment, action, parentNode->GetHirerchyNo() + 1);

					parentNode->AddChild(addNode);
				}
			}
			//�e�����Ȃ���
			else {
				// �匳���Ȃ����
				if (root == nullptr)
				{
					// �匳�ɐݒ肷��
					root = new NodeBase<EnemyT>(entryName, nullptr, nullptr, priority, selectRule, judgment, action, 1);
				}
			}
		}



		// ���s
		NodeBase<EnemyT>* Run(NodeBase<EnemyT>* actionNode, BehaviorData<EnemyT>* data, float elapsedTime)
		{
			// �m�[�h���s
			// todo ������EnemyT�ɂ������G���[�o��̂͂Ȃ��H
			typename ActionBase<EnemyT>::State state = actionNode->Run(elapsedTime);

			// ����I��
			if (state == ActionBase<EnemyT>::State::Complete)
			{
				// �V�[�P���X�̓r�����𔻒f

				//�V�[�P���X���Ă�e�m�[�h
				NodeBase<EnemyT>* sequenceNode = data->PopSequenceNode();

				// �r������Ȃ��Ȃ�I��
				if (sequenceNode == nullptr)
				{
					// todo ������nullptr��Ԃ��̂����Ɠs������
					return nullptr;
				}
				else {
					// �r���Ȃ炻������n�߂�
					return SequenceBack(sequenceNode, data);
				}
			}
			// ���s�͏I��
			else if (state == ActionBase<EnemyT>::State::Failed) {
				return nullptr;
			}

			// ����ێ�
			return actionNode;
		}


		// AI_pattern_�ɂ���č\�z���邱�ƃr�w�C�r�A�c���[��ς���
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
			_ASSERT_EXPR(false, L"AI�̃p�^�[�����s���ł�");
		}

		void Update() override
		{
			// �����Ńr�w�C�r�A�c���[�ɂ��s���J��

#if 1 �������ōs���邩����

		// ���ݎ��s����m�[�h�������
			if (active_node != nullptr)
			{
				// �r�w�C�r�A�c���[����m�[�h�����s�B
				active_node = Run(active_node, behaviorData, Dante::Time::Timer::Instance().GetDeltaTime());
			}
			// ���ݎ��s����Ă���m�[�h���������
			if (active_node == nullptr)
			{
				// ���Ɏ��s����m�[�h�𐄘_����B
				active_node = ActiveNodeInference(behaviorData);
			}

# else ���Ă̂���
		// ���ݎ��s����Ă���m�[�h���������
			if (active_node == nullptr)
			{
				// ���Ɏ��s����m�[�h�𐄘_����B
				active_node = ActiveNodeInference(behaviorData);
			}
			// ���ݎ��s����m�[�h�������
			if (active_node != nullptr)
			{
				// �r�w�C�r�A�c���[����m�[�h�����s�B
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
		// �m�[�h�S�폜
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
		// ���[�g�m�[�h(�c���[�̑匳)
		NodeBase<EnemyT>* root;

		// ��ɃV�[�N�G���X�̍ۂɎg�p����B���ԃm�[�h�Ƃ��̒��ԃm�[�h�̃X�e�b�v���Ȃǂ�ێ�����B
		BehaviorData<EnemyT>* behaviorData = new BehaviorData<EnemyT>;

		// �I�����ꂽ���[�m�[�h(���ۂ̓���)������B
		//��ɉ��������ĂȂ��ƑS�����삵�Ȃ����ƂɂȂ�̂Ńo�O��B
		NodeBase<EnemyT>* active_node = nullptr;

		// todo �e���v���[�g��
		// �r�w�C�r�A�c���[�̎g�p��(�^��Character�ɂ��ׂ��H)
		EnemyT* owner;

	private:
		// AI�̃p�^�[��
		int AI_pattern;

#if 0 �{�XAI
		void BossEnemy_AI_Protocol()
		{
			//�r�w�C�r�A�c���[�̍\�z
			this->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority);
			{
				// ���S
				this->AddNode("Root", "death", 0, BehaviorTree::SelectRule::Non, new DeathJudge(owner), new DeathAction(owner));
				// ��_�����A�N�V����(���񂾂��Ƃ͏u�Ԉړ�)
				this->AddNode("Root", "ReAction", 2, BehaviorTree::SelectRule::Sequence, new FearJudge(owner));
				{
					// ����
					this->AddNode("ReAction", "SmallFear", 0, BehaviorTree::SelectRule::Non, nullptr, new FearAction(owner));
					// �����Ƀe���|�[�g
					this->AddNode("ReAction", "Teleport(ReAction)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 20.0f));
					// �o��
					this->AddNode("ReAction", "Awake", 0, BehaviorTree::SelectRule::Non, new ToAwakeJudge(owner), new AwakeAction(owner));

				}
				// ��퓬
				this->AddNode("Root", "NonBattle", 1, BehaviorTree::SelectRule::Priority, new IdleJudge(owner));
				{
					this->AddNode("NonBattle", "Idle(NonBattle)", 0, BehaviorTree::SelectRule::Non, nullptr, new IdleAction(owner));
				}
				// �퓬��
				this->AddNode("Root", "Battle", 3, BehaviorTree::SelectRule::Priority, new BattleJudge(owner));
				{
					// �ҋ@�n
					this->AddNode("Battle", "IdleState", 2, BehaviorTree::SelectRule::Random);
					{
						// ����
						this->AddNode("IdleState", "Walk", 0, BehaviorTree::SelectRule::Non, nullptr, new WalkAction(owner));

					}
					// �U���n
					this->AddNode("Battle", "AttackState", 1, BehaviorTree::SelectRule::Priority, new AttackStateJudge(owner));
					{
						// �ߐڍU��
						this->AddNode("AttackState", "NearNormalAttack", 1, BehaviorTree::SelectRule::Non, new NearAttackStateJudge(owner, 4.0f), new NormalAttack(owner));

						// �U���������_���Œ��I����
						this->AddNode("AttackState", "RandomAttack", 2, BehaviorTree::SelectRule::Random);
						{
							// �u�Ԉړ����ĕK�E�Z
							this->AddNode("RandomAttack", "Teleport->SpecialAttack", 0, BehaviorTree::SelectRule::Sequence, new SpecialAttackJudge(owner));
							{
								// �e���|�[�g
								this->AddNode("Teleport->SpecialAttack", "Teleport", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 1.0f));
								// �K�E�Z
								this->AddNode("Teleport->SpecialAttack", "SpecialAttack", 0, BehaviorTree::SelectRule::Non, nullptr, new SpecialAttackAction(owner));
							}
							// �e���|�[�g����̋ߋ����U��
							this->AddNode("RandomAttack", "Teleport->NormalAttack(Attack)", 0, BehaviorTree::SelectRule::Sequence);
							{
								// �e���|�[�g
								this->AddNode("Teleport->NormalAttack(Attack)", "Teleport(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 1.0f));
								// �ߐڍU��
								this->AddNode("Teleport->NormalAttack(Attack)", "NormalAttack(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new NormalAttack(owner));
							}
							// �e���|�[�g����̑O�i�U��
							this->AddNode("RandomAttack", "Teleport->MoveAttack(Attack)", 0, BehaviorTree::SelectRule::Sequence);
							{
								// �e���|�[�g
								this->AddNode("Teleport->MoveAttack(Attack)", "Teleport(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 5.0f));
								// �O�i�U��
								this->AddNode("Teleport->MoveAttack(Attack)", "MoveAttack(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new MoveAttackAction(owner));
							}
							// �e���|�[�g����̑O�i�U���~2
							this->AddNode("RandomAttack", "Teleport->MoveAttack*2(Attack)", 0, BehaviorTree::SelectRule::Sequence, new AwakeJudge(owner));
							{
								// �e���|�[�g
								this->AddNode("Teleport->MoveAttack*2(Attack)", "Teleport(Attack*1)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 5.0f));
								// �O�i�U��
								this->AddNode("Teleport->MoveAttack*2(Attack)", "MoveAttack(Attack*1)", 0, BehaviorTree::SelectRule::Non, nullptr, new MoveAttackAction(owner));
								// �e���|�[�g
								this->AddNode("Teleport->MoveAttack*2(Attack)", "Teleport(Attack*2)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 5.0f));
								// �O�i�U��
								this->AddNode("Teleport->MoveAttack*2(Attack)", "MoveAttack(Attack*2)", 0, BehaviorTree::SelectRule::Non, nullptr, new MoveAttackAction(owner));
							}
							// ���e���|�[�g����̃G�t�F�N�g�U��
							this->AddNode("RandomAttack", "HighTeleport->EffectAttack(Attack)", 0, BehaviorTree::SelectRule::Sequence);
							{
								// �e���|�[�g
								this->AddNode("HighTeleport->EffectAttack(Attack)", "FirstHighTeleport(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 5.0f, 5.0f));
								// �ߐڍU��
								this->AddNode("HighTeleport->EffectAttack(Attack)", "EffectAttack(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new EffectAttack(owner));
								// �e���|�[�g
								this->AddNode("HighTeleport->EffectAttack(Attack)", "EndHighTeleport(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 15.0f));
							}
							// ���e���|�[�g����̃G�t�F�N�g�U���~2
							this->AddNode("RandomAttack", "HighTeleport->EffectAttack(Attack)�~2", 0, BehaviorTree::SelectRule::Sequence);
							{
								// �e���|�[�g
								this->AddNode("HighTeleport->EffectAttack(Attack)�~2", "FirstHighTeleport(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 5.0f, 5.0f));
								// �G�t�F�N�g�U��
								this->AddNode("HighTeleport->EffectAttack(Attack)�~2", "FirstEffectAttack(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new EffectAttack(owner));
								// �G�t�F�N�g�U��
								this->AddNode("HighTeleport->EffectAttack(Attack)�~2", "SecondEffectAttack(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new EffectAttack(owner));
								// �e���|�[�g
								this->AddNode("HighTeleport->EffectAttack(Attack)�~2", "EndHighTeleport(Attack)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 15.0f));
							}

							// �e���|�[�g
							this->AddNode("RandomAttack", "Teleport", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 1.0f));

							// �J�E���^�[�U��
							this->AddNode("RandomAttack", "Counter", 0, BehaviorTree::SelectRule::Non, nullptr, new CounterAttackAction(owner));
							// �G�t�F�N�g�U��
							this->AddNode("RandomAttack", "EffectAttack", 0, BehaviorTree::SelectRule::Non, nullptr, new EffectAttack(owner));
						}
					}
					// �p���B�n
					this->AddNode("Battle", "ParryCombo", 0, BehaviorTree::SelectRule::Sequence, new ParryJudge(owner));
					{
						// �p���B�A�N�V����
						this->AddNode("ParryCombo", "Parry", 0, BehaviorTree::SelectRule::Non, nullptr, new ParryAction(owner));

						//�p���B��̃A�N�V����
						this->AddNode("ParryCombo", "After(ParryCombo)", 0, BehaviorTree::SelectRule::Random);
						{
							// �����Ƀe���|�[�g�̂�
							this->AddNode("After(ParryCombo)", "Teleport", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 20.0f));

							// �e���|�[�g����̋ߋ����U��
							this->AddNode("After(ParryCombo)", "Teleport->NormalAttack(Parry)", 0, BehaviorTree::SelectRule::Sequence);
							{
								// �e���|�[�g
								this->AddNode("Teleport->NormalAttack(Parry)", "Teleport(ParryCombo)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 1.0f));
								// �ߐڍU��
								this->AddNode("Teleport->NormalAttack(Parry)", "NormalAttack(ParryCombo)", 0, BehaviorTree::SelectRule::Non, nullptr, new NormalAttack(owner));
							}
							// �e���|�[�g����̑O�i�U��
							this->AddNode("After(ParryCombo)", "Teleport->MoveAttack(Parry)", 0, BehaviorTree::SelectRule::Sequence);
							{
								// �e���|�[�g
								this->AddNode("Teleport->MoveAttack(Parry)", "Teleport(ParryCombo)", 0, BehaviorTree::SelectRule::Non, nullptr, new TeleportAction(owner, 5.0f));
								// �ߐڍU��
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
		//	//�r�w�C�r�A�c���[�̍\�z
		//	this->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority);
		//	{
		//		// ����
		//		this->AddNode("Root", "SmallFear", 0, BehaviorTree::SelectRule::Non, new FearJudge(owner), new FearAction(owner, FLT_MAX));
		//		// �ҋ@
		//		this->AddNode("Root", "PunchingBag", 1, BehaviorTree::SelectRule::Non, nullptr, new PunchingBagMode(owner));

		//	}
		//}

		void CatFishAIProtocol()
		{
			AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority);
			{
				AddNode("Root", "NonBattle", 0, BehaviorTree::SelectRule::Random);
				{
					// todo ���s���ɋC�ɂȂ���݂̂����ȃ|�C���g��u���Ƃ��Ď��E�ɂ��ꂪ�ڂ����炻�����ɍs���݂����Ȃ��Ƃ��������B
					// �ǂ��������ɂ��̂��̂����āA�ǂ�Ȋ����ł������ɍs���̂����p�����[�^(���i�Ƃ�)�Ō̍����o�������B
					//�G�[�W�F���g�A�[�L�e�N�`���̓��������Ă����ňӎv������g�p
					AddNode("NonBattle", "Wandering", 0, BehaviorTree::SelectRule::Non, nullptr, new WanderingAction(owner));
				}
				//AddNode("Root", "Battle", 1, BehaviorTree::SelectRule::Priority, new BattleJudge(owner));
				{
				    
				}
			}
		}

		void CatFishTutorialAIProtocol()
		{
			//�r�w�C�r�A�c���[�̍\�z
			this->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority);
			{
				// ����
				this->AddNode("Root", "SmallFear", 0, BehaviorTree::SelectRule::Non, new FearJudge(owner), new CatFishFearAction(owner, FLT_MAX));
				// �ҋ@
				this->AddNode("Root", "PunchingBag", 1, BehaviorTree::SelectRule::Non, nullptr, new CatFishPunchingBagMode(owner));

			}
		}

	};

    

    // �m�[�h
	template <class EnemyT>
	class NodeBase
	{
	public:
		// �R���X�g���N�^
		NodeBase(std::string name, NodeBase* parent, NodeBase* sibling, int priority,
			BehaviorTree<EnemyT>::SelectRule selectRule, JudgmentBase<EnemyT>* judgment, ActionBase<EnemyT>* action, int hierarchyNo) :
			name(name), parent(parent), sibling(sibling), priority(priority),
			selectRule(selectRule), judgment(judgment), action(action), hierarchyNo(hierarchyNo),
			children(NULL)
		{
		}
		// �f�X�g���N�^
		~NodeBase()
		{
			delete judgment;
			delete action;
		}


		// ���O�Q�b�^�[
		std::string GetName() { return name; }

		// �e�m�[�h�Q�b�^�[
		NodeBase* GetParent() { return parent; }

		// �q�m�[�h�Q�b�^�[
		NodeBase* GetChild(int index)
		{
			if (children.size() <= index)
			{
				return nullptr;
			}
			return children.at(index);
		}

		// �q�m�[�h�Q�b�^�[(����)
		NodeBase* GetLastChild()
		{
			if (children.size() == 0)
			{
				return nullptr;
			}

			return children.at(children.size() - 1);
		}

		// �q�m�[�h�Q�b�^�[(�擪)
		NodeBase* GetTopChild()
		{
			if (children.size() == 0)
			{
				return nullptr;
			}

			return children.at(0);
		}


		// �Z��m�[�h�Q�b�^�[
		NodeBase* GetSibling() { return sibling; }

		// �K�w�ԍ��Q�b�^�[
		int GetHirerchyNo() { return hierarchyNo; }

		// �D�揇�ʃQ�b�^�[
		int GetPriority() { return priority; }

		// �e�m�[�h�Z�b�^�[
		void SetParent(NodeBase* parent) { this->parent = parent; }

		// �q�m�[�h�ǉ�
		void AddChild(NodeBase* child) { children.push_back(child); }

		// �Z��m�[�h�Z�b�^�[
		void SetSibling(NodeBase* sibling) { this->sibling = sibling; }

		// �s���f�[�^�������Ă��邩
		bool HasAction() { return action != nullptr ? true : false; }

		// ���s�۔���
		bool Judgment()
		{
			// judgment�����邩���f�B����΃����o�֐�Judgment()���s�������ʂ����^�[���B
			if (judgment != nullptr)
			{
				return judgment->Judgment();
			}
			return true;
		}


		// �D�揇�ʑI��
		NodeBase* SelectPriority(std::vector<NodeBase*>* list)
		{
			NodeBase* selectNode = nullptr;
			int priority = INT_MAX;

			// ��ԗD�揇�ʂ������m�[�h��T����selectNode�Ɋi�[
			//�S�Ă̑I�����̐������J��Ԃ�
			for (NodeBase* node : *list)
			{
				//��ԗD�揇�ʂ��������̂�T��
				if (node->GetPriority() < priority)
				{
					priority = node->GetPriority();

					// �ŏI�I�Ɉ�ԗD�揇�ʂ̍������̂�I������B
					selectNode = node;
				}
			}

			return selectNode;
		}

		// �����_���I��
		NodeBase* SelectRandom(std::vector<NodeBase*>* list)
		{
			int selectNo = 0;
			// list�̃T�C�Y�ŗ������擾����selectNo�Ɋi�[
			selectNo = rand() % list->size();

			// list��selectNo�Ԗڂ̎��Ԃ����^�[��
			return (*list).at(selectNo);
		}

		// �V�[�P���X�I��
		NodeBase* SelectSequence(std::vector<NodeBase*>& list, BehaviorData<EnemyT>* data)
		{
			// �q�m�[�h�̂ǂ��܂ŏ���������
			int step = 0;

			// �w�肳��Ă��钆�ԃm�[�h�̃V�[�P���X���ǂ��܂Ŏ��s���ꂽ���擾����
			step = data->GetSequenceStep(name);

			// ���ԃm�[�h�ɓo�^����Ă���m�[�h���ȏ�̏ꍇ(�Ō�܂ŃV�[�P���X�ŏ��������ꍇ)�A
			if (step >= children.size())
			{
				// ���[���ɂ���ď�����؂�ւ���
				// ���[����BehaviorTree::SelectRule::SequentialLooping�̂Ƃ��͍ŏ�������s���邽�߁Astep��0����
				if (selectRule == BehaviorTree<EnemyT>::SelectRule::SequentialLooping)
				{
					step = 0;
				}

				// ���[����BehaviorTree::SelectRule::Sequence�̂Ƃ��͎��Ɏ��s�ł���m�[�h���Ȃ����߁Anullptr�����^�[��
				if (selectRule == BehaviorTree<EnemyT>::SelectRule::Sequence)
				{
					return nullptr;
				}
			}

			// �V�[�P���X������s�\���X�g���ۑ�����Ă�����
			if (!data->GetSequenceList().empty())
				//���̎��s�\���X�g���g���B
				list = data->GetSequenceList();

			// ���s�\���X�g�ɓo�^����Ă���f�[�^�̐��������[�v���s��
			for (; step < children.size(); step++) {
				for (auto itr = list.begin(); itr != list.end(); itr++)
				{
					// �q�m�[�h�����s�\���X�g�Ɋ܂܂�Ă��邩
					if (children.at(step)->GetName() == (*itr)->GetName())
					{
						// ���݂̎��s�m�[�h�̕ۑ��A���Ɏ��s����X�e�b�v�̕ۑ����s������A
						// ���݂̃X�e�b�v�ԍ��̃m�[�h�����^�[������
						// (���̃t���[���ł����̃m�[�h�����s���Ă邩���ʂ��邽��)

						// ���s���̒��ԃm�[�h��ۑ��B�V�[�P���X���Ă�e�{�l�m�[�h�B
						data->PushSequenceNode(this);

						// �A�܂��A���Ɏ��s���钆�ԃm�[�h�ƃX�e�b�v����ۑ�����
						// �@�ۑ��ɂ�data->SetSequenceStep�֐����g�p�B
						// �@�ۑ��f�[�^�͒��ԃm�[�h�̖��O�Ǝ��̃X�e�b�v���ł�(step + 1)
						data->SetSequenceStep(this->name, step + 1);

						// �B�X�e�b�v�ԍ��ڂ̎q�m�[�h�����s�m�[�h�Ƃ��ă��^�[������
						return children.at(step);
					}
				}
			}

			// �w�肳�ꂽ���ԃm�[�h�Ɏ��s�\�m�[�h���Ȃ��̂�nullptr�����^�[������
			return nullptr;
		}



		// �m�[�h����(�����Ǝ����̑S�Ă̎q�̖��O���r���Č�������)
		NodeBase* SearchNode(std::string searchName)
		{
			// ���O����v
			if (name == searchName)
			{
				return this;
			}
			else {
				// �q�m�[�h�Ō���
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



		// �m�[�h���_(���̍s�������ɑI�ʂ���)
		NodeBase* Inference(BehaviorData<EnemyT>* data)
		{
			// ���̍s���̃��X�g�A�����ɂł���I�����̍s��������
			std::vector<NodeBase*> list;
			// ���肳�ꂽ�m�[�h������
			NodeBase* result = nullptr;

			// �V�[�P���X������s�\���X�g����Ȃ�(�V�[�P���X���ĂȂ��A�܂��͂܂��o�^����ĂȂ����Ă���)
			if (data->GetSequenceList().empty())
			{
				// ���̍s���̑I�������i���Ă�

				// children�̐��������[�v���s���B
				for (int i = 0; i < children.size(); i++)
				{
					// children.at(i)->judgment��nullptr�łȂ����
					if (children.at(i)->judgment != nullptr)
					{
						// children.at(i)->judgment->Judgment()�֐������s���Ature�ł����
						// list��children.at(i)��ǉ����Ă���
						if (children.at(i)->judgment->Judgment())
						{
							list.emplace_back(children.at(i));
						}
					}
					else {
						// ����N���X���Ȃ���Ζ������ɒǉ�
						list.emplace_back(children.at(i));
					}
				}
			}

			// �I�����[���Ńm�[�h����
			switch (selectRule)
			{
				// �D�揇��
			case BehaviorTree<EnemyT>::SelectRule::Priority:
				result = SelectPriority(&list);
				break;
				// �����_��
			case BehaviorTree<EnemyT>::SelectRule::Random:
				result = SelectRandom(&list);
				break;
				// �V�[�P���X
			case BehaviorTree<EnemyT>::SelectRule::Sequence:
			case BehaviorTree<EnemyT>::SelectRule::SequentialLooping:
				result = SelectSequence(list, data);
				break;
			}

			// �����ŃA�N�V����������ΐ��_���I��
			// �Ⴄ�ꍇ�͂���Ɉ���̃m�[�h����I������T��
			// �A�N�V������������܂ł��B�A�N�V�������Ȃ�������o�O��̂Œ��ӁB
			if (result != nullptr)
			{
				// �s��������ΏI��
				if (result->HasAction() == true)
				{
					return result;
				}
				else {
					// ���܂����m�[�h�ł܂����_�J�n
					result = result->Inference(data);
				}
			}


			return result;
		}


		// ���s(�I�ʂ��ꂽ�s�������s�Aupdate�݂����Ȃ��)
		ActionBase<EnemyT>::State Run(float elapsedTime)
		{
			// action�����邩���f�B����΃����o�֐�Run()���s�������ʂ����^�[���B
			if (action != nullptr)
			{
				return action->Run(elapsedTime);
			}

			return ActionBase<EnemyT>::State::Failed;
		}




		// �q�m�[�h�A�����̃m�[�h�ł͂Ȃ�����
		//�e�m�[�h�ɂ����̉��̊K�w�̃m�[�h�����B
		//(����̊K�w�����A�����܂ł͂Ȃ��B)
		// todo vector�ɂ��Ă���Ă��Ƃ͓����m�[�h������邩��A�������m�����������̂ł́H
		std::vector<NodeBase*>		children;

	protected:

		std::string					name;			// ���O
		BehaviorTree<EnemyT>::SelectRule	selectRule;		// �I�����[��

		// ���肷��A����̓��e�B�p�����Ĕ���̓��e�����R�ɍ��B
		//nullptr�Ȃ画�肵�Ȃ��̂ő��I�����̌��ɂȂ�
		JudgmentBase<EnemyT>* judgment;	    // ����N���X

		// ���ۂ̓����A����̓��e�B�p�����čs���̓��e�����R�ɍ��B
		//nullptr�Ȃ���ۂ̓����͂Ȃ��������ł͂Ȃ��͂�
		ActionBase<EnemyT>* action;			// ���s�N���X

		unsigned int				priority;		// �D�揇��
		NodeBase<EnemyT>* parent;			// �e�m�[�h
		NodeBase<EnemyT>* sibling;		// �Z��m�[�h
		int							hierarchyNo;	// �K�w�ԍ�
	};


	// Behavior�ۑ��f�[�^
	template <class EnemyT>
	class BehaviorData
	{
	public:
		// �R���X�g���N�^
		BehaviorData() { Init(); }
		// �V�[�P���X�m�[�h�̃v�b�V��
		void PushSequenceNode(NodeBase<EnemyT>* node) { sequenceStack.push(node); }
		// �V�[�P���X�m�[�h�̃|�b�v
		NodeBase<EnemyT>* PopSequenceNode()
		{
			// ��Ȃ�NULL
			if (sequenceStack.empty() != 0)
			{
				return nullptr;
			}

			// �V�[�P���X���Ă�e�m�[�h���擾
			NodeBase<EnemyT>* node = sequenceStack.top();

			if (node != nullptr)
			{
				// ���o�����f�[�^���폜
				sequenceStack.pop(); //	pop�͗v�f���폜����֐�
			}

			return node;
		}


		// �V�[�P���X�X�e�b�v�̃Q�b�^�[
		int GetSequenceStep(std::string name)
		{
			if (runSequenceStepMap.count(name) == 0)
			{
				runSequenceStepMap.insert(std::make_pair(name, 0));
			}

			return runSequenceStepMap.at(name);
		}

		// �V�[�P���X�X�e�b�v�̃Z�b�^�[
		void SetSequenceStep(std::string name, int step)
		{
			runSequenceStepMap.at(name) = step;
		}

		// ������
		void Init()
		{
			runSequenceStepMap.clear();
			while (sequenceStack.size() > 0)
			{
				//�X�^�b�N�g�b�v(�R���e�i�̖�����)�̗v�f���폜����
				sequenceStack.pop();
			}
		}


		// �V�[�P���X������s�\���X�g�擾
		const std::vector<NodeBase<EnemyT>*>& GetSequenceList() { return sequence_list; }
	private:

		/* �V�[�P���X������s�\���X�g
		 *���ނ̂����̓V�[�P���X�Ŏ��̍s���Ɉڍs����Ƃ��ɖ���
		 *���_���Ă��܂�����A�V�[�P���X�Ƃ킩�����Ȃ���ڂ�
		 *���s�\���X�g�ɓ������A�N�V������ێ�����Failed�ɂȂ�Ȃ�����
		 *���ׂĎ��s����`�ɂ���B
		 *(�����͋��ނ̃p�^�[���Ǝg�������ł���悤�ɂȂ��Ă����������A
		 *�g���@��邩�킩��񂯂�)
		 */
		std::vector<NodeBase<EnemyT>*> sequence_list{};

		// �}�����ꂽ������v�f�����o��(�{��ςރC���[�W)
		// �V�[�P���X���s���Ă钆�ԃm�[�h���X�^�b�N(�e�{�l)
		std::stack<NodeBase<EnemyT>*> sequenceStack;

		// ���s���̒��ԃm�[�h�̃X�e�b�v���L�^
		std::map<std::string, int> runSequenceStepMap;
	};


}
