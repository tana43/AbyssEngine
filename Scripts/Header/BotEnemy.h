#pragma once
#include "BaseEnemy.h"

namespace AbyssEngine
{
    class SkeletalMesh;
}

class BotEnemy : public BaseEnemy
{
public:
    enum class AnimState
    {
        Idle,
        Rolling,
        Walk,
        Jump, 
        Search,
        Attack,
        Attack_Assult,//�O�i���U��
    };

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
    void Update()override;

    void DrawDebug()override;

    //�v���C���[�����G
    bool SearchTarget();

public:

private:
    std::shared_ptr<AbyssEngine::SkeletalMesh> model_;


    //���G�͈�
    float searchAreaRadius_ = 5.0f;

    //�^���I�Ȏ���p�@�G�����E���ɂ��邩�𔻒f����
    float degreeFov_ = 150.0f;

    //��̎���p���O�`�P�ɐ��K����������
    float fov_;
};

