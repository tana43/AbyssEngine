#include "CharacterManager.h"
#include "Engine.h"
#include "Actor.h"
#include <algorithm>

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

    ImGui::SliderFloat("Rot Speed", &baseRotSpeed_, 0.0f, 1000.0f);
    ImGui::SliderFloat("Max Rot Speed", &Max_Rot_Speed,0.0f,1000.0f);
    ImGui::SliderFloat("Min Rot Speed", &Min_Rot_Speed,0.0f,1000.0f);

    return true;
}

void Character::TurnY(Vector3 dir, bool smooth)
{
    if (dir.LengthSquared() == 0)return;

    //XZ���݂̂̃x�N�g�����K��
    Vector2 d = { dir.x,dir.z };
    d.Normalize();
    dir = { d.x,0,d.y };
    auto forward = transform_->GetForward();

    float rotY = transform_->GetRotation().y;

    //���ϒl����ŏI�I�Ɍ��������p�x���v�Z����
    float dot = forward.Dot(dir);
    if (dot > 0.99f)return;
    float rotAmount = acosf(dot);

    float rotSpeed = 0;

    //�Ȃ߂炩�ȐU��������A�����ɐU�������
    if (smooth)
    {
        //���ϒl��0~1�͈̔͂�
        dot = dot + 1.0f;

        rotSpeed = baseRotSpeed_ * rotAmount;
        
    }
    else
    {
        rotSpeed = rotAmount;
    }

    //��]���x����
    std::clamp(rotSpeed, Min_Rot_Speed, Max_Rot_Speed);

    //�O�ς�Y���̂݋��߁A��]���������߂�
    float crossY = forward.z * dir.x - forward.x * dir.z;
    if (crossY < 0) rotSpeed = -rotSpeed;
    
    rotY += rotSpeed * Time::deltaTime_;
    transform_->SetRotationY(rotY);
}

void Character::Move()
{
    UpdateVelocity();
    UpdateMove(); 
}

void Character::UpdateVelocity()
{
    //���͍X�V
    {
        if (moveVec_.LengthSquared() > 0.01f)
        {
            velocity_ = velocity_ + moveVec_ * (acceleration_ * Time::deltaTime_);

            //���x����
            if (velocity_.Length() > Max_Speed)
            {
                velocity_.Normalize();
                velocity_ = velocity_ * Max_Speed;
            }
        }
        else//��������
        {
            //���͒l���قڂȂ��ꍇ�͌�������
            velocity_ = velocity_ - (velocity_ * (deceleration_ * Time::deltaTime_));

            //���x���O�ɋ߂��Ƃ��͊��S�ɂO�ɂ���
            if (velocity_.LengthSquared() > 0.01f)
            {
                velocity_ = {};
            }
        }
    }
}

void Character::UpdateMove()
{
    //���W�X�V
    {
        auto pos = transform_->GetPosition();
        pos = pos + velocity_ * Time::deltaTime_;
        transform_->SetPosition(pos);
    }

    //��]
    TurnY(velocity_);
}
