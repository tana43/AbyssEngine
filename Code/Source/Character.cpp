#include "CharacterManager.h"
#include "Engine.h"
#include "Actor.h"
#include "imgui/imgui.h"

using namespace AbyssEngine;

void Character::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //�}�l�[�W���[�̓o�^�Ə�����
    Engine::characterManager_->AddCharacter(std::static_pointer_cast<Character>(shared_from_this()));
    actor_ = actor;
    transform_ = actor->GetTransform();
}

bool Character::DrawImGui()
{
    ImGui::DragFloat3("Velocity", &velocity_.x);

    float speed = velocity_.Length();
    ImGui::SliderFloat("Speed",&speed,0.0f,Max_Speed);

    ImGui::DragFloat("Max Speed", &Max_Speed,0.1f,0.1f);

    ImGui::SliderFloat("Accel", &acceleration_, 0.0f, 10.0f);
    ImGui::SliderFloat("Decel", &deceleration_, 0.0f, 10.0f);

    return true;
}

void Character::Turn(Vector3 dir, bool smooth)
{
    dir.Normalize();
    auto forward = transform_->GetForward();
    auto rot = transform_->GetRotation()

    //���ϒl����ŏI�I�Ɍ��������������v�Z����
    float dot = forward.Dot(dir);
    float rotSpeed = dot

    //�O�ς�Y���̂݋��߁A��]���������߂�
    float crossY = forward.z * dir.x - forward.x * dir.z;
    if(crossY > 0)
}
