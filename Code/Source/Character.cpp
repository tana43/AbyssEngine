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

void AbyssEngine::Character::Update()
{
    UpdateMove();
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

    ImGui::DragFloat3("Move Vec", &moveVec_.x, 0.05f, -1.0f, 1.0f);

    ImGui::DragFloat("Terrain Radius", &terrainRadius_, 0.01f, 0.0f);
    ImGui::DragFloat("Terrain Center Offset", &terrainCenterOffset_, 0.01f, 0.0f);
    ImGui::DragFloat("Terrain Step Offset", &terrainStepOffset_, 0.01f, 0.0f);

    return true;
}

bool Character::Jump(const float& jumpPower)
{
    //���݋󒆂ɂ���ꍇ�̓W�����v���Ȃ�
    if (!onGround_)return false;

    velocity_.y += jumpPower;

    onGround_ = false;

    return true;
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
    float rotAmount = acosf(dot);
    if (dot > 0.999f)
    {
        return;
    }

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

    if (smooth)
    {
        rotY += rotSpeed * Time::deltaTime_;
    }
    else
    {
        rotY += rotSpeed;
    }
    transform_->SetRotationY(rotY);
}

void AbyssEngine::Character::TurnY(Vector3 dir, const float& speed, bool smooth)
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
    float rotAmount = DirectX::XMConvertToDegrees(acosf(dot));

    float rotSpeed = 0;

    //�Ȃ߂炩�ȐU��������A�����ɐU�������
    if (smooth)
    {
        //���ϒl��0~1�͈̔͂�
        dot = dot + 1.0f;

        rotSpeed = speed * rotAmount * Time::deltaTime_;

        if (rotSpeed > rotAmount)
        {
            rotSpeed = rotAmount;
        }
    }
    else
    {
        rotSpeed = rotAmount;
    }

    //�O�ς�Y���̂݋��߁A��]���������߂�
    float crossY = forward.z * dir.x - forward.x * dir.z;
    if (crossY < 0) rotSpeed = -rotSpeed;
    
    rotY += rotSpeed;
    
    transform_->SetRotationY(rotY);
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
        if (enableGravity_)
        {
            //�d�͂ɂ�鑬�͍X�V
            velocity_.y += Gravity * weight_ * Time::deltaTime_;
        }

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
    if (enableAutoTurn_)
    {
        TurnY(Vector3(velocity_.x,0,velocity_.z));
    }
}

void Character::UpdateHorizontalMove()
{
    //if (velocity_.x * velocity_.x + velocity_.z * velocity_.z == 0)return;

    //�����Ȉړ���̍��W���擾
    const Vector3 pos = transform_->GetPosition();
    Vector3 move =
    {
        velocity_.x * Time::deltaTime_,
        0,
        velocity_.z * Time::deltaTime_
    };

    //�A�j���[�^�[�������Ă���Ȃ烋�[�g���[�V�����ɂ��ړ��l�����Z
    if (const auto& animator = actor_->GetComponent<Animator>())
    {
        Vector3 rootMotion = animator->GetRootMotionMove();
        move.x += rootMotion.x;
        move.z += rootMotion.z;
        animator->SetRootMotionMove(Vector3(0, rootMotion.y, 0));
    }

    const Vector3 moveVector = move;

    Vector3 moved = pos + moveVector;//�ړ���̍��W

    //�X�t�B�A�L���X�g
    if (sphereCast_)
    {
        //���S�_
        Vector3 centerOffset = { 0,terrainRadius_ + terrainCenterOffset_,0 };

        // �L���X�g�ʂ��Z�o
        float distance = moveVector.Length();
        if (distance > 0.000001f)
        {
            //  �ǂ�菭����O�Ŏ~�܂��Ăق����̂ŁA�L���X�g�ʂ𑝂₷(skinWidth)
            distance += terrainSkinWidth_;

            Vector3 origin = transform_->GetPosition() + centerOffset;
            Vector3 direction = moveVector.Normalize();
            Vector3 hitPosition, hitNormal;

            //�X�t�B�A�L���X�g
            const auto& stage = StageManager::Instance().GetActiveStage();
            if (stage.get() && stage->SphereCast(
                origin, direction, terrainRadius_, distance, hitPosition, hitNormal, true))
            {
                //  �L���X�g�ʂ𑝂₵�����������炷(skinWidth)
                distance -= terrainSkinWidth_;

                //�ړ�
                moved = pos + direction * distance;

                //���菈��
                {
                    //  ����ړ����ǂɉ����悤�ړ��x�N�g�����Z�o
                    float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(hitNormal, moveVector));
                    const Vector3 move = moveVector - hitNormal * dot;
                    moved = pos + move;
                }
            }
        }
    }
    //���C�L���X�gver
    else
    {
        Vector3 hit;//�������q�b�g�����Ƃ���̍��W
        Vector3 hitNormal;//�������q�b�g�����ʂ̖@��

        //���C�̊J�n�n�_�����グ
        static constexpr float correctionY = 0.5f;//�␳�l
        Vector3 start = pos;
        start.y += correctionY;
        Vector3 end = moved;
        end.y += correctionY;


        //�n�`����
        const auto& stage = StageManager::Instance().GetActiveStage();
        if (stage.get() && stage->RayCast(start, end, hit, hitNormal))
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
    }

    //���W�X�V
    transform_->SetPositionX(moved.x);
    transform_->SetPositionZ(moved.z);
}

void Character::UpdateVerticalMove()
{
    //���x���牽�̔�������Ȃ������Ƃ��̈ړ���̍��W���擾
    const Vector3 pos = transform_->GetPosition();
    float moveY = velocity_.y * Time::deltaTime_;

    //�A�j���[�^�[�������Ă���Ȃ烋�[�g���[�V�����ɂ��ړ��l�����Z
    if (const auto& animator = actor_->GetComponent<Animator>())
    {
        Vector3 rootMotion = animator->GetRootMotionMove();
        moveY += rootMotion.y;
        animator->SetRootMotionMove(Vector3(rootMotion.x, 0, rootMotion.z));

    }

    //�ړ���̍��W
    Vector3 moved = {pos.x,pos.y + moveY,pos.z};

    //�X�t�B�A�L���X�g
    if (sphereCast_)
    {
        //���S�_
        Vector3 centerOffset = { 0,terrainRadius_ + terrainStepOffset_,0 };

        // �L���X�g�ʂ��Z�o
        float distance = fabsf(moveY);
        if (distance > 0.000001f)
        {
            //�I�t�Z�b�g�����Z
            distance += terrainStepOffset_;

            Vector3 origin = transform_->GetPosition() + centerOffset;
            Vector3 direction = velocity_.y > 0 ? Vector3(0, 1, 0) : Vector3(0, -1, 0);
            Vector3 hitPosition, hitNormal;

            //�X�t�B�A�L���X�g
            const auto& stage = StageManager::Instance().GetActiveStage();
            if (stage.get() && stage->SphereCast(
                origin,direction,terrainRadius_,distance,hitPosition,hitNormal,true))
            {
                //�X���[�v�p�x���Z�o
                DirectX::XMVECTOR Up = DirectX::XMVectorSet(0, 1, 0, 0);
                float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Up, hitNormal));
                float angle = DirectX::XMConvertToDegrees(acosf(dot));

                //���n����
                Landing();
                distance -= terrainStepOffset_;
                moved.y = pos.y + direction.y * distance;

                //  �����p�x�ȓ��Ȃ犊�鏈���̓X�L�b�v
                if (angle > slopeLimit)
                {
                    Vector3 move = { 0, moveY, 0 };

                    //  ����ړ����ǂɉ����悤�ړ��x�N�g�����Z�o
                    float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(hitNormal,move));
                    move = DirectX::XMVectorSubtract(move, DirectX::XMVectorScale(hitNormal, dot));
                    moved = pos + move;
                }
            }
        }
    }
    //���C�L���X�g
    else
    {
        Vector3 hit;//�������q�b�g�����Ƃ���̍��W
        Vector3 hitNormal;//�������q�b�g�����ʂ̖@��

        //���C�̊J�n�n�_�����グ
        float correctionY = terrainStepOffset_;//�␳�l
        Vector3 start = pos;
        start.y += correctionY;
        Vector3 end = moved;

        //���������ɒn�`����
        const auto& stage = StageManager::Instance().GetActiveStage();
        if (stage.get() && stage->RayCast(start, end, hit, hitNormal))
        {
            //���n����
            Landing();
            moved.y = hit.y;
        }
    }

    transform_->SetPosition(moved);
}

void Character::Landing()
{
    velocity_.y = 0.0f;
    onGround_ = true;
}
