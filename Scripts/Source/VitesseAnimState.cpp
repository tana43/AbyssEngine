#include "VitesseAnimState.h"
#include "Vitesse.h"
#include "Actor.h"

using namespace AbyssEngine;

void VitesseAnimState::AnimGroundMove::Initialize()
{
    //�A�j���[�V�����ݒ�
    owner_->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Run_Move));

    isPlayFallAnim_ = false;
}

void VitesseAnimState::AnimGroundMove::Update(float deltaTime)
{
    const auto& vi = owner_->GetActor()->GetComponent<Vitesse>();

    //���n���Ă��邩
    if (vi->GetOnGround())
    {
        if (isPlayFallAnim_)
        {
            owner_->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Run_Move));
            isPlayFallAnim_ = false;
        }

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
            Vector3 moveDirection;
            velocityXZ.Normalize(moveDirection);

            Vector2 result;

            //���ςɂ��v�Z
            float dot = forward.Dot(moveDirection);
            dot = std::clamp(dot, -1.0f, 1.0f);
            float radian = acosf(dot);
            float crossY = forward.z * moveDirection.x - forward.x * moveDirection.z;

            //���E����
            //���ϒl���P�̂Ƃ��ɂ��̂܂ܐ������Ђ�����Ԃ��Ă��܂��Ƒ傫���p�x���ς���Ă��܂��̂ŁA������l�����Čv�Z����
            if (crossY < 0)radian = DirectX::XM_2PI - radian;
            result = { sinf(radian),cosf(radian) };

            //max���x�̂W���𖞂��������x�Ȃ�u�����h�l���P��
            float weight = fminf(velocityXZ.Length() / (vi->GetMaxHorizontalSpeed() * 0.8f), 1.0f);
            result = result * weight;

            vi->GetGroundMoveAnimation()->SetBlendWeight(result);

            //�ړ������ɑ��
            moveDirection = { result.x,0,result.y };

            /*if ((std::fetestexcept(FE_DIVBYZERO)))
            {
                _ASSERT_EXPR(false, L"Div by 0");
            }*/
        }
    }
    else
    {
        //�󒆂ɂ���Ȃ痎�����[�V�����Đ�
        if (!isPlayFallAnim_)
        {
            owner_->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Ground_Fall), 0.5f);
            isPlayFallAnim_ = true;
        }
    }

    
}

void VitesseAnimState::AnimGroundMove::Finalize()
{
    
}

void VitesseAnimState::AnimFlight::Initialize()
{
    //�A�j���[�V�����ݒ�
    owner_->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Flight_Move),0.5f);
}

void VitesseAnimState::AnimFlight::Update(float deltaTime)
{
    const auto& vi = owner_->GetActor()->GetComponent<Vitesse>();
    const auto& velo = vi->GetVelocity();

    //�u�����h�A�j���[�V������Weight�X�V
    vi->GetFlightAnimation()->SetMoveVec(vi->GetMoveVec());
    Vector3 velocityXZ = { velo.x,0,velo.z };
    if (velocityXZ.LengthSquared() < 0.01f)
    {
        vi->GetFlightAnimation()->GetBlendSpace2D()->SetBlendWeight(Vector2(0, 0));
    }
    else
    {
        //�O�����Ɛi�s�����̍��̃x�N�g�����Z�o
        const auto& forward = owner_->GetTransform()->GetForward();
        Vector3 moveDirection;
        velocityXZ.Normalize(moveDirection);

        Vector2 result;

        //���ςɂ��v�Z
        float dot = forward.Dot(moveDirection);
        dot = std::clamp(dot, -1.0f, 1.0f);
        float radian = acosf(dot);
        float crossY = forward.z * moveDirection.x - forward.x * moveDirection.z;

        //���E����
        //���ϒl���P�̂Ƃ��ɂ��̂܂ܐ������Ђ�����Ԃ��Ă��܂��Ƒ傫���p�x���ς���Ă��܂��̂ŁA������l�����Čv�Z����
        if (crossY < 0)radian = DirectX::XM_2PI - radian;
        result = { sinf(radian),cosf(radian) };

        //�O���Z��h��
        if (velocityXZ.LengthSquared() == 0 || vi->GetMaxHorizontalSpeed() == 0)return;

        //max���x�̂W���𖞂��������x�Ȃ�u�����h�l���P��
        float weight = fminf(velocityXZ.Length() / (vi->GetMaxHorizontalSpeed() * 0.8f),1.0f);
        result = result * weight;

        vi->GetFlightAnimation()->GetBlendSpace2D()->SetBlendWeight(result);

        //�ړ������ɑ��
        vi->SetMoveDirection(Vector3(result.x,0,result.y));
    }

    if (fabsf(velo.y) > 0.1f)
    {
        float result1D = velo.y / vi->GetMaxVerticalSpeed();
        result1D = std::clamp(result1D, -1.0f, 1.0f);
        vi->GetFlightAnimation()->GetBlendSpace1D()->SetBlendWeight(result1D);
    }
    else
    {
        vi->GetFlightAnimation()->GetBlendSpace1D()->SetBlendWeight(0.0f);
    }
}

void VitesseAnimState::AnimFlight::Finalize()
{
}

void VitesseAnimState::AnimHighSpeedFlight::Initialize()
{
    //�A�j���[�V�����ݒ�
    owner_->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::HighSpeedFlight_Move));
}

void VitesseAnimState::AnimHighSpeedFlight::Update(float deltaTime)
{
    const auto& vi = owner_->GetActor()->GetComponent<Vitesse>();
    const auto& velo = vi->GetVelocity();

    //�u�����h�A�j���[�V������Weight�X�V
    vi->GetHighSpeedFlightAnimation()->SetMoveVec(vi->GetMoveVec());
    Vector3 velocityXZ = { velo.x,0,velo.z };
    if (velocityXZ.LengthSquared() < 0.01f)
    {
        vi->GetHighSpeedFlightAnimation()->GetBlendSpace2D()->SetBlendWeight(Vector2(0, 1));
    }
    else
    {
        //�O�����Ɛi�s�����̍��̃x�N�g�����Z�o
        const auto& forward = owner_->GetTransform()->GetForward();
        Vector3 moveDirection;
        velocityXZ.Normalize(moveDirection);

        Vector2 result;

        //���ςɂ��v�Z
        float dot = forward.Dot(moveDirection);
        dot = std::clamp(dot,-1.0f,1.0f);
        float radian = acosf(dot);

        float crossY = forward.z * moveDirection.x - forward.x * moveDirection.z;

        //���E����
        //���ϒl���P�̂Ƃ��ɂ��̂܂ܐ������Ђ�����Ԃ��Ă��܂��Ƒ傫���p�x���ς���Ă��܂��̂ŁA������l�����Čv�Z����
        if (crossY < 0)radian = DirectX::XM_2PI - radian;
        result = { sinf(radian),cosf(radian) };

        result.Normalize();
        vi->GetHighSpeedFlightAnimation()->GetBlendSpace2D()->SetBlendWeight(result);

        //�ړ������ɑ��
        vi->SetMoveDirection(Vector3(result.x, 0, result.y));

        if (std::isnan(result.x) || std::isnan(result.y))
        {
            _ASSERT_EXPR(false, L"Divide by 0");
        }
    }

    if (fabsf(velo.y) > 0.1f)
    {
        float result1D = velo.y / vi->GetMaxVerticalSpeed();
        result1D = std::clamp(result1D, -1.0f, 1.0f);
        vi->GetHighSpeedFlightAnimation()->GetBlendSpace1D()->SetBlendWeight(result1D);
    }
    else
    {
        vi->GetHighSpeedFlightAnimation()->GetBlendSpace1D()->SetBlendWeight(0.0f);
    }

    
}

void VitesseAnimState::AnimHighSpeedFlight::Finalize()
{

}
