#include "VitesseAnimState.h"
#include "Vitesse.h"
#include "Actor.h"

using namespace AbyssEngine;

void VitesseAnimState::AnimGroundMove::Initialize()
{
    //�A�j���[�V�����ݒ�
    owner_->PlayAnimation(static_cast<int>(Vitesse::AnimState::Run_Move));
}

void VitesseAnimState::AnimGroundMove::Update()
{
    const auto& vi = owner_->GetActor()->GetComponent<Vitesse>();
    const auto velo = vi->GetVelocity();
    Vector3 velocityXZ = { velo.x,0,velo.z };
    if (fabsf(velocityXZ.LengthSquared()) < 0.01f)
    {
        vi->GetGroundMoveAnimation()->SetBlendWeight(Vector2(0, 0));
    }
    else
    {
        //�O�����Ɛi�s�����̍��̃x�N�g�����Z�o
        const auto& forward = owner_->GetTransform()->GetForward();
        //const auto& right = transform_->GetRight();
        Vector3 moveDirection;
        velocityXZ.Normalize(moveDirection);

        Vector2 result;

        //���ςɂ��v�Z
        float dot = acosf(forward.Dot(moveDirection));
        float crossY = forward.z * moveDirection.x - forward.x * moveDirection.z;

        //���E����
        //���ϒl���P�̂Ƃ��ɂ��̂܂ܐ������Ђ�����Ԃ��Ă��܂��Ƒ傫���p�x���ς���Ă��܂��̂ŁA������l�����Čv�Z����
        if (crossY < 0)dot = DirectX::XM_2PI - dot;
        result = { sinf(dot),cosf(dot) };
        result = result * (velocityXZ.Length() / vi->GetMaxHorizontalSpeed());

        vi->GetGroundMoveAnimation()->SetBlendWeight(result);

        //�ړ������ɑ��
        moveDirection = { result.x,0,result.y };
    }
}

void VitesseAnimState::AnimGroundMove::Finalize()
{
    
}

void VitesseAnimState::AnimFlyMove::Initialize()
{
    //�A�j���[�V�����ݒ�
    owner_->PlayAnimation(static_cast<int>(Vitesse::AnimState::Run_Move));
}

void VitesseAnimState::AnimFlyMove::Update()
{
    const auto& vi = owner_->GetActor()->GetComponent<Vitesse>();
    const auto velo = vi->GetVelocity();
    Vector3 velocityXZ = { velo.x,0,velo.z };
    if (fabsf(velocityXZ.LengthSquared()) < 0.01f)
    {
        vi->GetFlyMoveAnimation()->GetBlendSpace2D()->SetBlendWeight(Vector2(0, 0));
    }
    else
    {
        //�O�����Ɛi�s�����̍��̃x�N�g�����Z�o
        const auto& forward = owner_->GetTransform()->GetForward();
        //const auto& right = transform_->GetRight();
        Vector3 moveDirection;
        velocityXZ.Normalize(moveDirection);

        Vector2 result;

        //���ςɂ��v�Z
        float dot = acosf(forward.Dot(moveDirection));
        float crossY = forward.z * moveDirection.x - forward.x * moveDirection.z;

        //���E����
        //���ϒl���P�̂Ƃ��ɂ��̂܂ܐ������Ђ�����Ԃ��Ă��܂��Ƒ傫���p�x���ς���Ă��܂��̂ŁA������l�����Čv�Z����
        if (crossY < 0)dot = DirectX::XM_2PI - dot;
        result = { sinf(dot),cosf(dot) };
        result = result * (velocityXZ.Length() / vi->GetMaxHorizontalSpeed());

        vi->GetFlyMoveAnimation()->GetBlendSpace2D()->SetBlendWeight(result);

        //�ړ������ɑ��
        moveDirection = { result.x,0,result.y };
    }
}

void VitesseAnimState::AnimFlyMove::Finalize()
{
}
