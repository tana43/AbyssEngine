#include "CharacterManager.h"
#include "Engine.h"
#include "Actor.h"
#include "StageManager.h"

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
    //�������Ƃ��Ȃ����͏������Ȃ�
    if (velocity_.LengthSquared() * Time::deltaTime_ < 0.01f)return;
    
    //���x���牽�̔�������Ȃ������Ƃ��̈ړ���̍��W���擾
    const Vector3 pos = transform_->GetPosition(); 
    const Vector3 moveVector = velocity_ * Time::deltaTime_;
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
    if(stage->RayCast(start,end,hit,hitNormal))
    {
#if 0
        //���œ����Ȃ��悤�ɂ��Ă݂�
        hit.y -= correctionY;
        hit = hit + hitNormal * 0.1f;
        moved = hit;
#else

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
#endif // 0
      
    

    //���W�X�V
    transform_->SetPosition(moved);

    //��]
    TurnY(velocity_);
}
