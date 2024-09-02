#pragma once
#include "ActionBase.h"

class BaseEnemy;
class BotEnemy;

#pragma region �G�l�~�[����
// �ʏ�U��
class BotAttackAction : public ActionBase<BotEnemy>
{
public:
	BotAttackAction(BotEnemy* owner) : ActionBase(owner) {}
	ActionBase::State Run()override;
private:
	float timer_ = 0;
};
#pragma endregion
