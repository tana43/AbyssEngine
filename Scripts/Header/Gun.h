#pragma once
#include "ScriptComponent.h"
#include "MathHelper.h"

namespace AbyssEngine
{
    class Projectile;
    class BillboardRenderer;
}

class Gun : public AbyssEngine::ScriptComponent
{
public:
    Gun() {}
    ~Gun() = default;

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    bool DrawImGui()override;
    void DrawDebug()override;

    void Update()override;

    /// <summary>
    /// �ˌ�
    /// </summary>
    /// <param name="hootingDirection">�e��ł���(���K�������邱��)</param>
    bool Shot(AbyssEngine::Vector3 shootingDirection);

    //�}�Y���t���b�V���̃G�t�F�N�g�̍��W�X�V
    void UpdateFlashEffect();

public:
    const AbyssEngine::Vector3& GetMuzzlePos() const { return muzzlePos_; }
    void SetMuzzlePos(const AbyssEngine::Vector3& pos) { muzzlePos_ = pos; }

    const bool& GetADS() const { return ADS_; }
    void SetADS(const bool& flag) { ADS_ = flag; }

    const float& GetPrecision() const { return precision_; }
    void SetPrecision(const float& precision) { precision_ = precision; }

    void SetRateOfFire(const float& rate) { rateOfFire_ = rate; }

    const std::shared_ptr<AbyssEngine::BillboardRenderer>& GetMuzzleFlashComponent() const { return muzzleFlashComponent_; }

    void SetColliderTag(const Collider::Tag& tag) { colliderTag_ = static_cast<unsigned int>(tag); }

private:
    //ADS(�X�R�[�v��`���Ă��邩)
    bool ADS_ = false;

    //�e���i�e�̐����ʒu�j
    AbyssEngine::Vector3 muzzlePos_;

    //���ˊ��o
    float rateOfFire_ = 0.05f;
    
    //���ˊԊu���v������^�C�}�[
    float rateTimer_;

    //�}�K�W���e��
    int magazineSize_ = 30;

    //�}�K�W�����̒e��
    int ammoInMag_ = magazineSize_ = 30;

    //�ǂꂾ���^�񒆂ɋ��������̐��x
    //float precision_ = 0.03f;
    float precision_ = 0.02f;

    //ADS���̐��x
    float adsPrecision = 0.01f;

    //�}�Y���t���b�V���p�̉摜
    std::shared_ptr<AbyssEngine::BillboardRenderer> muzzleFlashComponent_;
    
    //�G�t�F�N�g����
    float flashLifespan_ = 0.0f;
    float Max_Flash_Lifespan = 0.02f;

    //�e�ۂ̑傫��
    float bulletRadius_;

    //�e�ۂ̔���ɂ���^�O
    unsigned int colliderTag_;
};

