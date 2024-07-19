#include "Character.h"
#include "Engine.h"
#include "Actor.h"
#include "StageManager.h"
#include "SkeletalMesh.h"

#include <algorithm>

#include "imgui/imgui.h"

using namespace AbyssEngine;

void Character::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //�}�l�[�W���[�̓o�^�Ə�����
    ScriptComponent::Initialize(actor);
}

bool Character::DrawImGui()
{
    ImGui::DragFloat3("Velocity", &velocity_.x);

    float speed = velocity_.Length();
    ImGui::SliderFloat("Speed",&speed,0.0f,Max_Horizontal_Speed);

    ImGui::DragFloat("Max Horizontal Speed", &Max_Horizontal_Speed,0.1f,0.1f);
    ImGui::DragFloat("Max Vertical Speed", &Max_Vertical_Speed,0.1f,0.1f);

    ImGui::SliderFloat("Accel", &acceleration_, 0.0f, 100.0f);
    ImGui::SliderFloat("Decel", &deceleration_, 0.0f, 100.0f);

    ImGui::SliderFloat("Rot Speed", &baseRotSpeed_, 0.0f, 1000.0f);
    ImGui::SliderFloat("Max Rot Speed", &Max_Rot_Speed,0.0f,1000.0f);
    ImGui::SliderFloat("Min Rot Speed", &Min_Rot_Speed,0.0f,1000.0f);

    return true;
}

void Character::Jump(const float& jumpPower)
{
    //���݋󒆂ɂ���ꍇ�̓W�����v���Ȃ�
    if (!onGround_)return;

    velocity_.y += jumpPower;

    onGround_ = false;
}

void Character::TurnY(Vector3 dir, bool smooth)
{
    Vector2 d = { dir.x,dir.z };

    if (d.LengthSquared() == 0)return;

    //XZ���݂̂̃x�N�g�����K��
    d.Normalize();
    dir = { d.x,0,d.y };
    auto forward = transform_->GetForward();

    float rotY = transform_->GetRotation().y;

    //���ϒl����ŏI�I�Ɍ��������p�x���v�Z����
    float dot = forward.Dot(dir);
    if (dot > 0.999f)return;
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
    UpdateMove(); 
}

void Character::UpdateVelocity()
{
    //���͍X�V

    //��������
    {
        if (moveVec_.LengthSquared() > 0.01f)
        {
            if (onGround_)
            {
                velocity_ = velocity_ + moveVec_ * (acceleration_ * Time::deltaTime_);
            }
            else
            {
                //�󒆂ɂ���Ƃ��͐��䂵�Â炭����
                velocity_ = velocity_ + moveVec_ * (acceleration_ * Time::deltaTime_) * airborneCoefficient_;
            }

            //���x����
            Vector2 velocityXZ = { velocity_.x,velocity_.z };
            if (velocityXZ.Length() > Max_Horizontal_Speed)
            {
                velocityXZ.Normalize();
                velocityXZ = velocityXZ * Max_Horizontal_Speed;

                velocity_.x = velocityXZ.x;
                velocity_.z = velocityXZ.y;
            }
        }
        else//��������
        {
            //���͒l���قڂȂ��ꍇ�͌�������
            Vector3 velocityXZ = {velocity_.x,0,velocity_.z};
            Vector3 veloXZNormal;
            velocityXZ.Normalize(veloXZNormal);

            if (onGround_)
            {
                velocityXZ = velocity_ - (veloXZNormal * (deceleration_ * Time::deltaTime_));
            }
            else
            {
                velocityXZ = velocity_ - (veloXZNormal * (airResistance_ * Time::deltaTime_));
            }
            velocityXZ.y = 0;

            Vector3 veloNormal;
            velocity_.Normalize(veloNormal);
            velocityXZ.Normalize(veloXZNormal);

            //���Ε����̃x�N�g���ɂȂ��Ă��܂������x���x������Ȃ�A���x�����S�ɂO�ɂ���
            if (veloNormal.Dot(veloXZNormal) < 0 || velocityXZ.LengthSquared() < 0.1f)
            {
                velocity_ = { 0,velocity_.y,0 };
            }
            else
            {
                velocity_ = { velocityXZ.x,velocity_.y,velocityXZ.z };
            }
        }
    }

    //�c�����̑��͍X�V
    {
        //�d�͂ɂ�鑬�͍X�V
        velocity_.y += Gravity * weight_ * Time::deltaTime_;

        //���x����
        if (fabsf(velocity_.y) > Max_Vertical_Speed)
        {
            velocity_.y = velocity_.y > 0 ? Max_Vertical_Speed : -Max_Vertical_Speed;
        }
    }
}

void Character::UpdateMove()
{
    UpdateVelocity();

    //�������Ƃ��Ȃ����͏������Ȃ�
    //if (velocity_.LengthSquared() * Time::deltaTime_ < 0.01f)return;
    
    UpdateHorizontalMove();
    UpdateVerticalMove();

    //��]
    TurnY(Vector3(velocity_.x,0,velocity_.z));
}

void Character::UpdateHorizontalMove()
{
    if (velocity_.x * velocity_.x + velocity_.z * velocity_.z == 0)return;

    //���x���牽�̔�������Ȃ������Ƃ��̈ړ���̍��W���擾
    const Vector3 pos = transform_->GetPosition();
    const Vector3 moveVector =
    {
        velocity_.x * Time::deltaTime_,
        0,
        velocity_.z * Time::deltaTime_
    };

    Vector3 moved = pos + moveVector;//�ړ���̍��W

    Vector3 hit;//�������q�b�g�����Ƃ���̍��W
    Vector3 hitNormal;//�������q�b�g�����ʂ̖@��

    //���C�̊J�n�n�_�����グ
    static constexpr float correctionY = 1.0f;//�␳�l
    Vector3 start = pos;
    start.y += correctionY;
    Vector3 end = moved;
    end.y += correctionY;


    //�n�`����
    const auto& stage = StageManager::Instance().GetActiveStage();
    if (stage->RayCast(start, end, hit, hitNormal))
    {
        //�X�^�[�g���烌�C�����������ʒu�܂ł̃x�N�g��
        Vector3 endToHit = hit - end;

        //���ς��g���ǂ���x�N�g�����Z�o
        Vector3 projection = hitNormal * (hitNormal.Dot(endToHit));

        //�ǂ���ړ��̂��߂ɕK�v�Ȑ��l��p��
        Vector3 startWall = hit + hitNormal * 0.01f;//�ǂɂׂ�����������Ȃ��悤�ɂ��Ă���
        Vector3 endWall = end + projection + hitNormal * 0.01f;

        //�ǂ���ړ�
        if (stage->RayCast(startWall, endWall, hit, hitNormal))
        {
            //�ēx���������ꍇ�͕ǂ���͂��Ȃ�
            hit.y -= correctionY;
            moved = hit;
        }
        else
        {
            //������Ȃ�����
            endWall.y -= correctionY;
            moved = endWall;
        }

        //�ړ������ɉ����đ��x�������I�ɕύX
        {
            float moveDistance = Vector3(moved - pos).Length() / Time::deltaTime_;
            velocity_.Normalize();
            velocity_ = velocity_ * moveDistance;
        }
    }
    else
    {
        //������Ȃ�����
    }

    //���W�X�V
    transform_->SetPositionX(moved.x);
    transform_->SetPositionZ(moved.z);
}

void Character::UpdateVerticalMove()
{
    //���x���牽�̔�������Ȃ������Ƃ��̈ړ���̍��W���擾
    const Vector3 pos = transform_->GetPosition();
    const float moveY = velocity_.y * Time::deltaTime_;

    //�ړ���̍��W
    Vector3 moved = {pos.x,pos.y + moveY,pos.z};

    Vector3 hit;//�������q�b�g�����Ƃ���̍��W
    Vector3 hitNormal;//�������q�b�g�����ʂ̖@��

    //���C�̊J�n�n�_�����グ
    static constexpr float correctionY = 1.0f;//�␳�l
    Vector3 start = pos;
    start.y += correctionY;
    Vector3 end = moved;

    //���������ɒn�`����
    const auto& stage = StageManager::Instance().GetActiveStage();
    if (stage->RayCast(start,end,hit,hitNormal))
    {
        //���n����
        Landing();
        moved.y = hit.y;
    }

    transform_->SetPosition(moved);
}

void Character::Landing()
{
    velocity_.y = 0.0f;
    onGround_ = true;
}
