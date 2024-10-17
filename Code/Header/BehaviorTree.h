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


	// �r�w�C�r�A�c���[
	//���ꂪ�r�w�C�r�A�x�[�X�̑S�Ă̓���
	template <class T>
	class BehaviorTree : public ScriptComponent
	{
		/*
		* ���̃r�w�C�r�A�c���[�̑g�ݕ���ƁA�V�[�P���X�͒��ԃm�[�h���S�Ď��s�\�m�[�h��
		* �o�^����ĂȂ��Ƃ��܂����_����Ȃ�(SelectSequence�֐���for��������)
		* �m�[�h���V�[�P���X�ɂ���Ƃ��͑S�Ă̎q�m�[�h����{�I�ɐ�Βʂ�悤�ɂ���(Failed�A���s���s�͑��v)
		* ������������Priority���g�����B���܂���낤�B
		*/
	public:
		
		enum class SelectRule
		{
			Non,				// ����(���[�m�[�h�p)
			Priority,			// �D�揇��
			Sequence,			// �V�[�P���X
			SequentialLooping,	// �V�[�P���V�������[�s���O
			Random,				// �����_��
		};

	public:
		BehaviorTree() : root_(nullptr) {}

		~BehaviorTree() { NodeAllClear(root_); }

		// AI_pattern_�ɂ���č\�z���邱�ƃr�w�C�r�A�c���[��ς���
		void Initialize(const std::shared_ptr<Actor>& actor) override
		{
			ScriptComponent::Initialize(actor);
		}

		//�ʏ�X�V������ɍX�V�����邽�߂�UpdateBefore���g��
		void UpdateBefore() override
		{
			// �����Ńr�w�C�r�A�c���[�ɂ��s���J��

#if 1 �������ōs���邩����

		// ���ݎ��s����m�[�h�������
			if (activeNode_ != nullptr)
			{
				// �r�w�C�r�A�c���[����m�[�h�����s�B
				activeNode_ = Run(activeNode_, behaviorData_, actor_->GetDeltaTime());
			}
			// ���ݎ��s����Ă���m�[�h���������
			if (activeNode_ == nullptr)
			{
				// ���Ɏ��s����m�[�h�𐄘_����B
				activeNode_ = ActiveNodeInference(behaviorData_);
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


		// ���s�m�[�h�𐄘_����
		NodeBase<T>* ActiveNodeInference(BehaviorData<T>* data)
		{
			// �f�[�^�����Z�b�g���ĊJ�n
			data->Init();
			return root_->Inference(data);
		}


		// �V�[�P���X�m�[�h���琄�_�J�n
		NodeBase<T>* SequenceBack(NodeBase<T>* sequenceNode, BehaviorData<T>* data)
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
			int priority_, SelectRule selectRule_,
			JudgmentBase<T>* judgment_ = nullptr, 
			ActionBase<T>* action_ = nullptr)
		{
			// �e�̖��O���ݒ肳��Ă����
			if (parentName != "")
			{
				// �ݒ肳�ꂽ�e���O������
				NodeBase<T>* parentNode = root_->SearchNode(parentName);

				if (parentNode != nullptr)
				{
					// �Z��m�[�h
					NodeBase<T>* sibling_ = parentNode->GetLastChild();
					NodeBase<T>* addNode = new NodeBase<T>(entryName, parentNode, sibling_, priority_, selectRule_, judgment_, action_, parentNode->GetHirerchyNo() + 1);

					parentNode->AddChild(addNode);
				}
			}
			//�e�����Ȃ���
			else {
				// �匳���Ȃ����
				if (root_ == nullptr)
				{
					// �匳�ɐݒ肷��
					root_ = new NodeBase<T>(entryName, nullptr, nullptr, priority_, selectRule_, judgment_, action_, 1);
				}
			}
		}



		// ���s
		NodeBase<T>* Run(NodeBase<T>* actionNode, BehaviorData<T>* data, float elapsedTime)
		{
			// �m�[�h���s
			typename ActionBase<T>::State state = actionNode->Run(elapsedTime);

			// ����I��
			if (state == ActionBase<T>::State::Complete)
			{
				// �V�[�P���X�̓r�����𔻒f

				//�V�[�P���X���Ă�e�m�[�h
				NodeBase<T>* sequenceNode = data->PopSequenceNode();

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
			else if (state == ActionBase<T>::State::Failed) {
				return nullptr;
			}

			// ����ێ�
			return actionNode;
		}

	private:
		// �m�[�h�S�폜
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
		// ���[�g�m�[�h(�c���[�̑匳)
		NodeBase<T>* root_;

		// ��ɃV�[�N�G���X�̍ۂɎg�p����B���ԃm�[�h�Ƃ��̒��ԃm�[�h�̃X�e�b�v���Ȃǂ�ێ�����B
		BehaviorData<T>* behaviorData_ = new BehaviorData<T>;

		// �I�����ꂽ���[�m�[�h(���ۂ̓���)������B
		//��ɉ��������ĂȂ��ƑS�����삵�Ȃ����ƂɂȂ�̂Ńo�O��B
		NodeBase<T>* activeNode_ = nullptr;

		// todo �e���v���[�g��
		// �r�w�C�r�A�c���[�̎g�p��(�^��Character�ɂ��ׂ��H)
		std::weak_ptr<T> owner_;

	};


    // �m�[�h
	template <class T>
	class NodeBase
	{
	public:
		// �R���X�g���N�^
		NodeBase(std::string name_, NodeBase* parent_, NodeBase* sibling_, int priority_,
			BehaviorTree<T>::SelectRule selectRule_, JudgmentBase<T>* judgment_, ActionBase<T>* action_, int hierarchyNo_) :
			name_(name_), parent_(parent_), sibling_(sibling_), priority_(priority_),
			selectRule_(selectRule_), judgment_(judgment_), action_(action_), hierarchyNo_(hierarchyNo_),
			children_(NULL)
		{
		}
		// �f�X�g���N�^
		~NodeBase()
		{
			delete judgment_;
			delete action_;
		}


		// ���O�Q�b�^�[
		std::string GetName() { return name_; }

		// �e�m�[�h�Q�b�^�[
		NodeBase* GetParent() { return parent_; }

		// �q�m�[�h�Q�b�^�[
		NodeBase* GetChild(int index)
		{
			if (children_.size() <= index)
			{
				return nullptr;
			}
			return children_.at(index);
		}

		// �q�m�[�h�Q�b�^�[(����)
		NodeBase* GetLastChild()
		{
			if (children_.size() == 0)
			{
				return nullptr;
			}

			return children_.at(children_.size() - 1);
		}

		// �q�m�[�h�Q�b�^�[(�擪)
		NodeBase* GetTopChild()
		{
			if (children_.size() == 0)
			{
				return nullptr;
			}

			return children_.at(0);
		}


		// �Z��m�[�h�Q�b�^�[
		NodeBase* GetSibling() { return sibling_; }

		// �K�w�ԍ��Q�b�^�[
		int GetHirerchyNo() { return hierarchyNo_; }

		// �D�揇�ʃQ�b�^�[
		int GetPriority() { return priority_; }

		// �e�m�[�h�Z�b�^�[
		void SetParent(NodeBase* parent_) { this->parent_ = parent_; }

		// �q�m�[�h�ǉ�
		void AddChild(NodeBase* child) { children_.push_back(child); }

		// �Z��m�[�h�Z�b�^�[
		void SetSibling(NodeBase* sibling_) { this->sibling_ = sibling_; }

		// �s���f�[�^�������Ă��邩
		bool HasAction() { return action_ != nullptr ? true : false; }

		// ���s�۔���
		bool Judgment()
		{
			// judgment�����邩���f�B����΃����o�֐�Judgment()���s�������ʂ����^�[���B
			if (judgment_ != nullptr)
			{
				return judgment_->Judgment();
			}
			return true;
		}


		// �D�揇�ʑI��
		NodeBase* SelectPriority(std::vector<NodeBase*>* list)
		{
			NodeBase* selectNode = nullptr;
			int priority_ = INT_MAX;

			// ��ԗD�揇�ʂ������m�[�h��T����selectNode�Ɋi�[
			//�S�Ă̑I�����̐������J��Ԃ�
			for (NodeBase* node : *list)
			{
				//��ԗD�揇�ʂ��������̂�T��
				if (node->GetPriority() < priority_)
				{
					priority_ = node->GetPriority();

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
		NodeBase* SelectSequence(std::vector<NodeBase*>& list, BehaviorData<T>* data)
		{
			// �q�m�[�h�̂ǂ��܂ŏ���������
			int step = 0;

			// �w�肳��Ă��钆�ԃm�[�h�̃V�[�P���X���ǂ��܂Ŏ��s���ꂽ���擾����
			step = data->GetSequenceStep(name_);

			// ���ԃm�[�h�ɓo�^����Ă���m�[�h���ȏ�̏ꍇ(�Ō�܂ŃV�[�P���X�ŏ��������ꍇ)�A
			if (step >= children_.size())
			{
				// ���[���ɂ���ď�����؂�ւ���
				// ���[����BehaviorTree::SelectRule::SequentialLooping�̂Ƃ��͍ŏ�������s���邽�߁Astep��0����
				if (selectRule_ == BehaviorTree<T>::SelectRule::SequentialLooping)
				{
					step = 0;
				}

				// ���[����BehaviorTree::SelectRule::Sequence�̂Ƃ��͎��Ɏ��s�ł���m�[�h���Ȃ����߁Anullptr�����^�[��
				if (selectRule_ == BehaviorTree<T>::SelectRule::Sequence)
				{
					return nullptr;
				}
			}

			// �V�[�P���X������s�\���X�g���ۑ�����Ă�����
			if (!data->GetSequenceList().empty())
				//���̎��s�\���X�g���g���B
				list = data->GetSequenceList();

			// ���s�\���X�g�ɓo�^����Ă���f�[�^�̐��������[�v���s��
			for (; step < children_.size(); step++) {
				for (auto itr = list.begin(); itr != list.end(); itr++)
				{
					// �q�m�[�h�����s�\���X�g�Ɋ܂܂�Ă��邩
					if (children_.at(step)->GetName() == (*itr)->GetName())
					{
						// ���݂̎��s�m�[�h�̕ۑ��A���Ɏ��s����X�e�b�v�̕ۑ����s������A
						// ���݂̃X�e�b�v�ԍ��̃m�[�h�����^�[������
						// (���̃t���[���ł����̃m�[�h�����s���Ă邩���ʂ��邽��)

						// ���s���̒��ԃm�[�h��ۑ��B�V�[�P���X���Ă�e�{�l�m�[�h�B
						data->PushSequenceNode(this);

						// �A�܂��A���Ɏ��s���钆�ԃm�[�h�ƃX�e�b�v����ۑ�����
						// �@�ۑ��ɂ�data->SetSequenceStep�֐����g�p�B
						// �@�ۑ��f�[�^�͒��ԃm�[�h�̖��O�Ǝ��̃X�e�b�v���ł�(step + 1)
						data->SetSequenceStep(this->name_, step + 1);

						// �B�X�e�b�v�ԍ��ڂ̎q�m�[�h�����s�m�[�h�Ƃ��ă��^�[������
						return children_.at(step);
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
			if (name_ == searchName)
			{
				return this;
			}
			else {
				// �q�m�[�h�Ō���
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



		// �m�[�h���_(���̍s�������ɑI�ʂ���)
		NodeBase* Inference(BehaviorData<T>* data)
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
				for (int i = 0; i < children_.size(); i++)
				{
					// children.at(i)->judgment��nullptr�łȂ����
					if (children_.at(i)->judgment_ != nullptr)
					{
						// children.at(i)->judgment->Judgment()�֐������s���Ature�ł����
						// list��children.at(i)��ǉ����Ă���
						if (children_.at(i)->judgment_->Judgment())
						{
							list.emplace_back(children_.at(i));
						}
					}
					else {
						// ����N���X���Ȃ���Ζ������ɒǉ�
						list.emplace_back(children_.at(i));
					}
				}
			}

			// �I�����[���Ńm�[�h����
			switch (selectRule_)
			{
				// �D�揇��
			case BehaviorTree<T>::SelectRule::Priority:
				result = SelectPriority(&list);
				break;
				// �����_��
			case BehaviorTree<T>::SelectRule::Random:
				result = SelectRandom(&list);
				break;
				// �V�[�P���X
			case BehaviorTree<T>::SelectRule::Sequence:
			case BehaviorTree<T>::SelectRule::SequentialLooping:
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
		ActionBase<T>::State Run(float elapsedTime)
		{
			// action�����邩���f�B����΃����o�֐�Run()���s�������ʂ����^�[���B
			if (action_ != nullptr)
			{
				return action_->Run(elapsedTime);
			}

			return ActionBase<T>::State::Failed;
		}




		// �q�m�[�h�A�����̃m�[�h�ł͂Ȃ�����
		//�e�m�[�h�ɂ����̉��̊K�w�̃m�[�h�����B
		//(����̊K�w�����A�����܂ł͂Ȃ��B)
		// todo vector�ɂ��Ă���Ă��Ƃ͓����m�[�h������邩��A�������m�����������̂ł́H
		std::vector<NodeBase*>	children_;

	protected:

		std::string	name_;			// ���O
		BehaviorTree<T>::SelectRule selectRule_;		// �I�����[��

		// ���肷��A����̓��e�B�p�����Ĕ���̓��e�����R�ɍ��B
		//nullptr�Ȃ画�肵�Ȃ��̂ő��I�����̌��ɂȂ�
		JudgmentBase<T>* judgment_;	// ����N���X

		// ���ۂ̓����A����̓��e�B�p�����čs���̓��e�����R�ɍ��B
		//nullptr�Ȃ���ۂ̓����͂Ȃ��������ł͂Ȃ��͂�
		ActionBase<T>* action_;		// ���s�N���X

		unsigned int priority_;			// �D�揇��
		NodeBase<T>* parent_;		// �e�m�[�h
		NodeBase<T>* sibling_;		// �Z��m�[�h
		int	hierarchyNo_;				// �K�w�ԍ�
	};


	// Behavior�ۑ��f�[�^
	template <class T>
	class BehaviorData
	{
	public:
		// �R���X�g���N�^
		BehaviorData() { Init(); }
		// �V�[�P���X�m�[�h�̃v�b�V��
		void PushSequenceNode(NodeBase<T>* node) { sequenceStack_.push(node); }
		// �V�[�P���X�m�[�h�̃|�b�v
		NodeBase<T>* PopSequenceNode()
		{
			// ��Ȃ�NULL
			if (sequenceStack_.empty() != 0)
			{
				return nullptr;
			}

			// �V�[�P���X���Ă�e�m�[�h���擾
			NodeBase<T>* node = sequenceStack_.top();

			if (node != nullptr)
			{
				// ���o�����f�[�^���폜
				sequenceStack_.pop(); //	pop�͗v�f���폜����֐�
			}

			return node;
		}


		// �V�[�P���X�X�e�b�v�̃Q�b�^�[
		int GetSequenceStep(std::string name_)
		{
			if (runSequenceStepMap_.count(name_) == 0)
			{
				runSequenceStepMap_.insert(std::make_pair(name_, 0));
			}

			return runSequenceStepMap_.at(name_);
		}

		// �V�[�P���X�X�e�b�v�̃Z�b�^�[
		void SetSequenceStep(std::string name_, int step)
		{
			runSequenceStepMap_.at(name_) = step;
		}

		// ������
		void Init()
		{
			runSequenceStepMap_.clear();
			while (sequenceStack_.size() > 0)
			{
				//�X�^�b�N�g�b�v(�R���e�i�̖�����)�̗v�f���폜����
				sequenceStack_.pop();
			}
		}


		// �V�[�P���X������s�\���X�g�擾
		const std::vector<NodeBase<T>*>& GetSequenceList() { return sequenceList_; }
	private:

		/* �V�[�P���X������s�\���X�g
		 *���ނ̂����̓V�[�P���X�Ŏ��̍s���Ɉڍs����Ƃ��ɖ���
		 *���_���Ă��܂�����A�V�[�P���X�Ƃ킩�����Ȃ���ڂ�
		 *���s�\���X�g�ɓ������A�N�V������ێ�����Failed�ɂȂ�Ȃ�����
		 *���ׂĎ��s����`�ɂ���B
		 *(�����͋��ނ̃p�^�[���Ǝg�������ł���悤�ɂȂ��Ă����������A
		 *�g���@��邩�킩��񂯂�)
		 */
		std::vector<NodeBase<T>*> sequenceList_{};

		// �}�����ꂽ������v�f�����o��(�{��ςރC���[�W)
		// �V�[�P���X���s���Ă钆�ԃm�[�h���X�^�b�N(�e�{�l)
		std::stack<NodeBase<T>*> sequenceStack_;

		// ���s���̒��ԃm�[�h�̃X�e�b�v���L�^
		std::map<std::string, int> runSequenceStepMap_;
	};


}
