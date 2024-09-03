#pragma once
#include "Character.h"

class BaseEnemy : public AbyssEngine::Character
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

    void UpdateBegin()override;

    void DrawDebug()override;

    //�ڕW�n�_�Ɍ������Ĉړ� �ړI�ɓ��B������true��Ԃ�
    bool MoveToTarget();

    //�����̍��W�𒆐S�ɉ��ֈړ�
    void SideMove(const AbyssEngine::Vector3& centerPos, bool& moveRight);

    //�^�[�Q�b�g�ʒu�������_���ݒ�
    void SetRandomTargetPosition();

    //�꒣��ݒ�
    void SetTerritory(const AbyssEngine::Vector3& origin, float range);

public:
    const bool& GetCanAttack() const { return canAttack_; }
    void SetCanAttack(const bool& can) { canAttack_ = can; }

    const bool& GetCanDodge() const { return canDodge_; }
    void SetCanDodge(const bool& can) { canDodge_ = can; }

    const std::shared_ptr<AbyssEngine::Actor>& GetTargetActor() const { return targetActor_; }

    const AbyssEngine::Vector3& GetTargetPosition() const { return targetPosition_; }
    void SetTargetPosition(const AbyssEngine::Vector3& pos) { targetPosition_ = pos; }

    const float& GetTerritoryRange() const { return territoryRange_; }
    void SetTerritoryRange(const float& range) { territoryRange_ = range; }

    
protected:
    //�ڕW�n�_
    AbyssEngine::Vector3 targetPosition_;

    //�^�[�Q�b�g�i�v���C���[�j
    std::shared_ptr<AbyssEngine::Actor> targetActor_;

    //�꒣��ݒ�
    AbyssEngine::Vector3 territoryOrigin_;
    float territoryRange_ = 3.0f;

    //�ڕW�n�_�ɓ��B�������𔻒肷�邽�߂̉~���a
    float goalRadius_ = 0.5f;

    //�U���\��
    bool canAttack_;

    //����\��
    bool canDodge_;
};

