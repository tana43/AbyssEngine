#pragma once
#include "Effect.h"
#include "MathHelper.h"
#include "Component.h"

namespace AbyssEngine
{
    class SkeletalMesh;
}

class ThrusterEffect : public AbyssEngine::Component
{
private:
    //���ˍs��
    enum class Sequence
    {
        Standby,        //�ҋ@
        Ignition,       //�_��
        Burning,        //���˒�
        Extinguishing,  //����

        Boost,//�u�[�X�g�_�b�V��
    };

public:
    static int instanceCount_;
    ThrusterEffect();
    ~ThrusterEffect() {}

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

    bool CanMultiple()override { return true; }

    bool DrawImGui()override;

public:

    //�\�P�b�g�ɃA�^�b�`����
    void AttachSocket(const std::string& name);

    //�I�t�Z�b�g��]�l��ݒ�
    void SetOffsetPosition(const AbyssEngine::Vector3& position) { offsetPos_ = position; }
    void SetOffsetRotation(const AbyssEngine::Vector3& rotation) { offsetRot_ = rotation; }
    void SetOffsetScale(const AbyssEngine::Vector3& scale) { scale_ = scale; }
    void SetOffsetScale(const float& scale) { scale_ = { scale,scale,scale }; }

    void UpdateTransform();

    //�G�t�F�N�g�X�V�i�o�͂ɍ��킹���X�P�[�������Ȃǁj
    void UpdateInjection();

    //�G�t�F�N�g�Đ��J�n
    void Fire(const float normalPower = 0.6f);

    //�G�t�F�N�g��~
    void Stop();

    //�u�[�X�g�_�b�V���J�n�@�J�n���̏o�͂��w��
    void Boost(const float power = 1.5f);

private:
    void SaveToJson();
    bool doubleCheckFlag_ = false;

private:
    //�Đ����Ă���G�t�F�N�g���Ǘ����Ă���ϐ�
    Effekseer::Handle effekseerHandle_;

    //�G�t�F�N�g���������邽�߂̕ϐ��@�ǂݍ��񂾃G�t�F�N�g�̏����i�[���Ă���
     std::shared_ptr<AbyssEngine::Effect> effectEmitter_;

    std::shared_ptr<AbyssEngine::SkeletalMesh> attachModel_;
    std::string socketName_;
    AbyssEngine::Matrix socketMatrix_;

    //�I�t�Z�b�g�l
    AbyssEngine::Vector3 offsetPos_;
    AbyssEngine::Vector3 offsetRot_;
    AbyssEngine::Vector3 scale_ = {1.0f,1.0f,1.0f};

    //���݂̕��ˍs��
    Sequence sequence_ = Sequence::Standby;

    std::string name_ = "ThrusterEffect_";

public:
    //�o��(1~0�͈̔́A�X���X�^�[���˂̋���)
    float power_ = 0.0f;

    //�X���X�^�[�_�Α��x
    float ignitionSpeed_ = 15.0f;

    //�ʏ핬�˒��ɂ�����o��
    float normalPower_ = 0.3f;

    //�X���X�^�[���Α��x
    float extSpeed_ = 20.0f;

    //�u�[�X�g�_�b�V�����̏o��
    float boostPower_;
};

