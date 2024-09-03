#pragma once

// �s���������N���X
template <class T>
class ActionBase
{
public:
	ActionBase(T* owner):owner_(owner){}
	// ���s���
	enum class State
	{
		Run,		// ���s��
		Failed,		// ���s���s
		Complete,	// ���s����
	};

	// ���s����(�������z�֐�)
	virtual ActionBase::State Run(float elapsedTime) = 0;
protected:
	T* owner_;
	int step = 0;
};
