#pragma once
#include "ScriptComponent.h"
#include "MathHelper.h"

namespace AbyssEngine
{
    class Projectile;
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

public:
    const AbyssEngine::Vector3& GetMuzzlePos() const { return muzzlePos_; }
    void SetMuzzlePos(const AbyssEngine::Vector3& pos) { muzzlePos_ = pos; }

    const bool& GetADS() const { return ADS_; }
    void SetADS(const bool& flag) { ADS_ = flag; }
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
    float precision_ = 0.03f;

    //ADS���̐��x
    float adsPrecision = 0.01f;

};

