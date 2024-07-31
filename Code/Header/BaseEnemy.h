#pragma once
#include "Character.h"

class BaseEnemy : public AbyssEngine::Character
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

    void DrawDebug()override;

    //�ڕW�n�_�Ɍ������Ĉړ�
    void MoveToTarget();

    //�^�[�Q�b�g�ʒu�������_���ݒ�
    void SetRandomTargetPosition();

    //�꒣��ݒ�
    void SetTerritory(const AbyssEngine::Vector3& origin, float range);

protected:
    //�ڕW�n�_
    AbyssEngine::Vector3 targetPosition_;

    //�꒣��ݒ�
    AbyssEngine::Vector3 territoryOrigin_;
    float territoryRange_ = 3.0f;
};

