#pragma once

// 行動処理基底クラス
template <class T>
class ActionBase
{
public:
	ActionBase(T* owner):owner_(owner){}
	// 実行情報
	enum class State
	{
		Run,		// 実行中
		Failed,		// 実行失敗
		Complete,	// 実行成功
	};

	// 実行処理(純粋仮想関数)
	virtual ActionBase::State Run() = 0;
protected:
	T* owner_;
	int step = 0;
};
