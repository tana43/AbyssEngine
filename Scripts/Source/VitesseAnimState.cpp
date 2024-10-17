#include "VitesseAnimState.h"
#include "Vitesse.h"
#include "Actor.h"

using namespace AbyssEngine;

void VitesseAnimState::AnimGroundMove::Initialize()
{
    //アニメーション設定
    owner_->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Run_Move));

    isPlayFallAnim_ = false;
}

void VitesseAnimState::AnimGroundMove::Update(float deltaTime)
{
    const auto& vi = owner_->GetActor()->GetComponent<Vitesse>();

    //着地しているか
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
            //前方向と進行方向の差のベクトルを算出
            const auto& forward = owner_->GetTransform()->GetForward();
            Vector3 moveDirection;
            velocityXZ.Normalize(moveDirection);

            Vector2 result;

            //内積による計算
            float dot = forward.Dot(moveDirection);
            dot = std::clamp(dot, -1.0f, 1.0f);
            float radian = acosf(dot);
            float crossY = forward.z * moveDirection.x - forward.x * moveDirection.z;

            //左右判定
            //内積値が１のときにそのまま正負をひっくり返してしまうと大きく角度が変わってしまうので、それも考慮して計算する
            if (crossY < 0)radian = DirectX::XM_2PI - radian;
            result = { sinf(radian),cosf(radian) };

            //max速度の８割を満たした速度ならブレンド値を１に
            float weight = fminf(velocityXZ.Length() / (vi->GetMaxHorizontalSpeed() * 0.8f), 1.0f);
            result = result * weight;

            vi->GetGroundMoveAnimation()->SetBlendWeight(result);

            //移動方向に代入
            moveDirection = { result.x,0,result.y };

            /*if ((std::fetestexcept(FE_DIVBYZERO)))
            {
                _ASSERT_EXPR(false, L"Div by 0");
            }*/
        }
    }
    else
    {
        //空中にいるなら落下モーション再生
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
    //アニメーション設定
    owner_->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::Flight_Move),0.5f);
}

void VitesseAnimState::AnimFlight::Update(float deltaTime)
{
    const auto& vi = owner_->GetActor()->GetComponent<Vitesse>();
    const auto& velo = vi->GetVelocity();

    //ブレンドアニメーションのWeight更新
    vi->GetFlightAnimation()->SetMoveVec(vi->GetMoveVec());
    Vector3 velocityXZ = { velo.x,0,velo.z };
    if (velocityXZ.LengthSquared() < 0.01f)
    {
        vi->GetFlightAnimation()->GetBlendSpace2D()->SetBlendWeight(Vector2(0, 0));
    }
    else
    {
        //前方向と進行方向の差のベクトルを算出
        const auto& forward = owner_->GetTransform()->GetForward();
        Vector3 moveDirection;
        velocityXZ.Normalize(moveDirection);

        Vector2 result;

        //内積による計算
        float dot = forward.Dot(moveDirection);
        dot = std::clamp(dot, -1.0f, 1.0f);
        float radian = acosf(dot);
        float crossY = forward.z * moveDirection.x - forward.x * moveDirection.z;

        //左右判定
        //内積値が１のときにそのまま正負をひっくり返してしまうと大きく角度が変わってしまうので、それも考慮して計算する
        if (crossY < 0)radian = DirectX::XM_2PI - radian;
        result = { sinf(radian),cosf(radian) };

        //０除算を防ぐ
        if (velocityXZ.LengthSquared() == 0 || vi->GetMaxHorizontalSpeed() == 0)return;

        //max速度の８割を満たした速度ならブレンド値を１に
        float weight = fminf(velocityXZ.Length() / (vi->GetMaxHorizontalSpeed() * 0.8f),1.0f);
        result = result * weight;

        vi->GetFlightAnimation()->GetBlendSpace2D()->SetBlendWeight(result);

        //移動方向に代入
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
    //アニメーション設定
    owner_->PlayAnimation(static_cast<int>(Vitesse::AnimationIndex::HighSpeedFlight_Move));
}

void VitesseAnimState::AnimHighSpeedFlight::Update(float deltaTime)
{
    const auto& vi = owner_->GetActor()->GetComponent<Vitesse>();
    const auto& velo = vi->GetVelocity();

    //ブレンドアニメーションのWeight更新
    vi->GetHighSpeedFlightAnimation()->SetMoveVec(vi->GetMoveVec());
    Vector3 velocityXZ = { velo.x,0,velo.z };
    if (velocityXZ.LengthSquared() < 0.01f)
    {
        vi->GetHighSpeedFlightAnimation()->GetBlendSpace2D()->SetBlendWeight(Vector2(0, 1));
    }
    else
    {
        //前方向と進行方向の差のベクトルを算出
        const auto& forward = owner_->GetTransform()->GetForward();
        Vector3 moveDirection;
        velocityXZ.Normalize(moveDirection);

        Vector2 result;

        //内積による計算
        float dot = forward.Dot(moveDirection);
        dot = std::clamp(dot,-1.0f,1.0f);
        float radian = acosf(dot);

        float crossY = forward.z * moveDirection.x - forward.x * moveDirection.z;

        //左右判定
        //内積値が１のときにそのまま正負をひっくり返してしまうと大きく角度が変わってしまうので、それも考慮して計算する
        if (crossY < 0)radian = DirectX::XM_2PI - radian;
        result = { sinf(radian),cosf(radian) };

        result.Normalize();
        vi->GetHighSpeedFlightAnimation()->GetBlendSpace2D()->SetBlendWeight(result);

        //移動方向に代入
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
