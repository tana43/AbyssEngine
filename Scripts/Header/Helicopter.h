#pragma once
#include "Character.h"
#include "SkeletalMesh.h"

//�퓬�w��
class Helicopter : public AbyssEngine::Character
{
public:
    Helicopter() {}
    ~Helicopter() = default;

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    void Update()override;

    void DrawImGui()override;

private:
    //�v���y���͍s��v�Z�ŉ�]������
    std::shared_ptr<AbyssEngine::SkeletalMesh> model_;

    //���[�^�[�̉�]�o��
    float motorPower_ = 0.0f;


};

