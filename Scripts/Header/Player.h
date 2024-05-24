#pragma once
#include "Character.h"

namespace AbyssEngine
{
    class Camera;
}

class Player final : public AbyssEngine::Character
{
public:

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    void Update()override;
    bool DrawImGui()override;

private:
    void MoveUpdate();

private:
    std::shared_ptr<AbyssEngine::Camera> camera_;

private:
    float baseAcceleration_ = 10.0f;//��ƂȂ�����x�@����ɃR���g���[���[�̓��͒l�����v�Z������ۂ̉����x�����܂�
    const float Walk_Max_Speed = 3.0f;
    const float Run_Max_Speed = 5.0f;
};

