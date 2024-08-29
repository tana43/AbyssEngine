#pragma once

// é¿çsîªíË
template <class T> 
class JudgmentBase
{
public:
	JudgmentBase(T* owner) :owner_(owner) {}
	virtual bool Judgment() = 0;
protected:
	T* owner_;
};