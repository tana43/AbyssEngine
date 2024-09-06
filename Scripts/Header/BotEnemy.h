#pragma once
#include "BaseEnemy.h"

namespace AbyssEngine
{
    template <class T>
    class BehaviorTree;
    class SkeletalMesh;
    class Animator;
}

class Gun;

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
    void BehaviorTreeInitialize();

    void Update()override;

    bool DrawImGui()override;
    void DrawDebug()override;

    //�v���C���[�����G
    bool SearchTarget();

    //�G�C���ʒu���X�V
    void LockOn();

    //�ˌ�
    void Shot();

    //�U��
    void ReloadUpdate();

public:
    const std::shared_ptr<AbyssEngine::SkeletalMesh>& GetModel() const { return model_; }
    const std::shared_ptr<AbyssEngine::Animator>& GetAnimator() const;

    const float& GetLockOnTime() const { return LockOn_Time; }
    const float& GetLockOnShotTime() const { return LockOn_Shot_Time; }
    const float& GetAtkReloadTime() const { return Atk_Reload_Time; }
    
    const bool& GetIsTargetFind() const { return isTargetFind_; }
private:
    std::shared_ptr<AbyssEngine::SkeletalMesh> model_;

    //�r�w�C�r�A�c���[
    std::shared_ptr<AbyssEngine::BehaviorTree<BotEnemy>> aiTree_;

    //���G�͈�
    float searchAreaRadius_ = 5.0f;

    //�^���I�Ȏ���p�@�G�����E���ɂ��邩�𔻒f����
    float degreeFov_ = 150.0f;

    //��̎���p���O�`�P�ɐ��K����������
    float fov_;

    //�_���Ă���ʒu
    AbyssEngine::Vector3 aimPosition_;

    //�e�R���|�[�l���g
    std::shared_ptr<Gun> gunComponent_;

    //���b�N�I�����̎���
    float LockOn_Time = 4.0f;

    //���b�N�I���������甭�C�Ɋ|���鎞��
    float LockOn_Shot_Time = 1.0f;

    //�ēx�U���ł���̂Ɋ|���鎞��
    float Atk_Reload_Time = 3.5f;

    //�����[�h���Ԃ̌v���p�^�C�}�[
    float reloadTimer_ = 0;

    //���݃����[�h���Ă��邩
    bool reloadNow_ = false;

    //�v���C���[�������Ă��邩
    bool isTargetFind_ = false;
};

