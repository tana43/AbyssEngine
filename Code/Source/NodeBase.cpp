#include "JudgmentBase.h"
#include "NodeBase.h"
#include "EnemyBlueSlime.h"
#include "BehaviorData.h"
#include "ActionBase.h"



// �f�X�g���N�^
NodeBase::~NodeBase()
{
	delete judgment;
	delete action;
}
// �q�m�[�h�Q�b�^�[
NodeBase* NodeBase::GetChild(int index)
{
	if (children.size() <= index)//�C��
	{
		return nullptr;
	}
	return children.at(index);
}

// �q�m�[�h�Q�b�^�[(����)
NodeBase* NodeBase::GetLastChild()
{
	if (children.size() == 0)
	{
		return nullptr;
	}

	return children.at(children.size() - 1);
}

// �q�m�[�h�Q�b�^�[(�擪)
NodeBase* NodeBase::GetTopChild()
{
	if (children.size() == 0)
	{
		return nullptr;
	}

	return children.at(0);
}


// �m�[�h����
NodeBase* NodeBase::SearchNode(std::string searchName)
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

// �m�[�h���_
NodeBase* NodeBase::Inference(EnemyBlueSlime* enemy, BehaviorData* data)
{
	std::vector<NodeBase*> list;
	NodeBase* result = nullptr;

	// children�̐��������[�v���s���B
	for (int i = 0; i < children.size(); i++)
	{
		// children.at(i)->judgment��nullptr�łȂ����
		if (children.at(i)->judgment != nullptr)
		{
			// TODO 04_03 children.at(i)->judgment->Judgment()�֐������s���Ature�ł����
			// list��children.at(i)��ǉ����Ă���
			if (children.at(i)->judgment->Judgment())
			{
				list.emplace_back(children.at(i));
			}
		}
		else {
			// TODO 04_03 ����N���X���Ȃ���Ζ������ɒǉ�
			list.emplace_back(children.at(i));
		}
	}

	// �I�����[���Ńm�[�h����
	switch (selectRule)
	{
		// �D�揇��
	case BehaviorTree::SelectRule::Priority:
		result = SelectPriority(&list);
		break;
		// �����_��
	case BehaviorTree::SelectRule::Random:
		result = SelectRandom(&list);
		break;
		// �V�[�P���X
	case BehaviorTree::SelectRule::Sequence:
	case BehaviorTree::SelectRule::SequentialLooping:
		result = SelectSequence(&list, data);
		break;
	}

	if (result != nullptr)
	{
		// �s��������ΏI��
		if (result->HasAction() == true)
		{
			return result;
		}
		else {
			// ���܂����m�[�h�Ő��_�J�n
			result = result->Inference(enemy, data);
		}
	}

	return result;
}

// �D�揇�ʂŃm�[�h�I��
NodeBase* NodeBase::SelectPriority(std::vector<NodeBase*>* list)
{
	NodeBase* selectNode = nullptr;
	int priority = INT_MAX;

	// TODO 04_04 list�Ɏ��s�\�ȃm�[�h���o�^����Ă���B
	//list�ɓo�^���ꂽ�m�[�h�̒��ň�ԗD�揇�ʂ��������̂�T����selectNode�Ɋi�[(�������������D��͍����j
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


// �����_���Ńm�[�h�I��
NodeBase* NodeBase::SelectRandom(std::vector<NodeBase*>* list)
{
	int selectNo = 0;
	// TODO 04_05 list�̃T�C�Y�ŗ������擾����selectNo�Ɋi�[
	selectNo = rand() % static_cast<int>(list->size());
	
	// list��selectNo�Ԗڂ̎��Ԃ����^�[��
	return (*list).at(selectNo);
}

// �V�[�P���X�E�V�[�P���V�������[�s���O�Ńm�[�h�I��
NodeBase* NodeBase::SelectSequence(std::vector<NodeBase*>* list, BehaviorData* data)
{
	int step = 0;

	// ���̒��ԃm�[�h�����Ɏ��s���ׂ�step���擾����
	step = data->GetSequenceStep(name);

	// ���ԃm�[�h�ɓo�^����Ă���m�[�h���ȏ�̏ꍇ�A
	if (step >= children.size())
	{
		// TODO 04_06 ���[���ɂ���ăV�[�P���V�����̏�����؂�ւ���i���[����this�ŎQ�Ɖ\�j
		//��̓I�Ȏ菇�͈ȉ��̒ʂ�
		// �@���[����BehaviorTree::SelectRule::SequentialLooping�̂Ƃ��͍ŏ�������s���邽�߁Astep��0����
		// �A���[����BehaviorTree::SelectRule::Sequence�̂Ƃ��͎��Ɏ��s�ł���m�[�h���Ȃ����߁Anullptr�����^�[��
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
	// ���s�\���X�g�ɓo�^����Ă���f�[�^�̐��������[�v���s��
	for (auto itr = list->begin(); itr != list->end(); itr++)
	{
		// �q�m�[�h�����s�\���X�g�Ɋ܂܂�Ă��邩
		if (children.at(step)->GetName() == (*itr)->GetName())
		{
			// TODO 04_06 ���݂̎��s�m�[�h���X�^�b�N�ɕۑ��A���Ɏ��s����X�e�b�v�̕ۑ����s������A
			// ���݂̃X�e�b�v�ԍ��̃m�[�h�����^�[�����Ȃ���
			//��̓I�Ȏ菇�͈ȉ��̒ʂ�
			// �@�X�^�b�N�ɂ�data->PushSequenceNode�֐����g�p����B�ۑ�����f�[�^�͎��s���̒��ԃm�[�h�ł���this�B
			// �A���ɒ��ԃm�[�h�Ɓu���̃X�e�b�v���v��ۑ�����
			// �@�X�e�b�v���̕ۑ��ɂ�data->SetSequenceStep�֐����g�p���鎖�B
			// �@�����͂��̒��ԃm�[�h�̖��O(this->name)�ƃX�e�b�v��+1�ł�(step + 1)
			// �B�X�e�b�v�ԍ��ڂ̎q�m�[�h�����s�m�[�h�Ƃ��ă��^�[������
			data->PushSequenceNode(this);
			data->SetSequenceStep(this->name, step + 1);
			return list->at(step);

		}
	}
	// �w�肳�ꂽ���ԃm�[�h�Ɏ��s�\�m�[�h���Ȃ��̂�nullptr�����^�[������
	return nullptr;
}

// ����
bool NodeBase::Judgment(EnemyBlueSlime* enemy)
{
	// TODO 04_07 judgment�����邩���f�B����΃����o�֐�Judgment()���s�������ʂ����^�[���B�i�啶����J�Ə�������j�ɒ��Ӂj
	if (judgment != nullptr)
	{
		return Judgment(enemy);
	}
	return true;
}

// �m�[�h���s
ActionBase::State NodeBase::Run(EnemyBlueSlime* enemy,float elapsedTime)
{
	// TODO 04_08 action�����邩���f�B����΃����o�֐�Run()���s�������ʂ����^�[���B
	if (action != nullptr)
	{
		return action->Run(elapsedTime);
	}

	return ActionBase::State::Failed;
}
