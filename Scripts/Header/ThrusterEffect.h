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
    };

public:
    ThrusterEffect();
    ~ThrusterEffect() {}

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

    bool CanMultiple()override { return true; }

    bool DrawImGui()override;

public:

    //�\�P�b�g�ɃA�^�b�`����
    void AttachSocket(std::string name);

    //�I�t�Z�b�g��]�l��ݒ�
    void SetRotation(const AbyssEngine::Vector3& rotation) { rotation_ = rotation; }
    void SetScale(const AbyssEngine::Vector3& scale) { scale_ = scale; }
    void SetScale(const float& scale) { scale_ = { scale,scale,scale }; }

    void UpdateTransform();

    //�G�t�F�N�g�X�V�i�o�͂ɍ��킹���X�P�[�������Ȃǁj
    void UpdateInjection();

    //�G�t�F�N�g�Đ��J�n
    void Fire();

    //�G�t�F�N�g��~
    void Stop();

private:
    //�Đ����Ă���G�t�F�N�g���Ǘ����Ă���ϐ�
    Effekseer::Handle effekseerHandle_;

    //�G�t�F�N�g���������邽�߂̕ϐ��@�ǂݍ��񂾃G�t�F�N�g�̏����i�[���Ă���
    static std::unique_ptr<Effect> effectEmitter_;

    std::shared_ptr<AbyssEngine::SkeletalMesh> attachModel_;
    std::string socketName_;
    DirectX::XMFLOAT4X4* socketGlobalTransform_;

    //�I�t�Z�b�g�l
    AbyssEngine::Vector3 rotation_;
    AbyssEngine::Vector3 scale_;

    //���݂̕��ˍs��
    Sequence sequence_ = Sequence::Standby;

public:
    //�o��(1~0�͈̔́A�X���X�^�[���˂̋���)
    float power_ = 0.0f;

    //�X���X�^�[�_�Α��x
    float ignitionSpeed_ = 5.0f;

    //�X���X�^�[���Α��x
    float ExtSpeed_ = 5.0f;
};

