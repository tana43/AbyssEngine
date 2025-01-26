#include "VitesseComboNode.h"
#include "AttackerSystem.h"
#include "Vitesse.h"
#include "Animator.h"

using namespace AbyssEngine;

void VitesseComboN1::Initialize()
{
    const auto owner = owner_.lock();
    if (!owner)return;

    //攻撃判定設定
    owner->BeginAttack("Slash_N_1");

    //アニメーション再生
    owner->PlayAnimation(Vitesse::AnimationIndex::Slash_N_1);
}

ComboSystemInterface::State VitesseComboN1::Run()
{
    const auto owner = owner_.lock();
    if (!owner)return ComboSystemInterface::State::NotCancellation;

    //アニメーション終了時にコンボ完了
    if (owner->GetAnimator()->GetAnimationFinished())
    {
        return ComboSystemInterface::State::Complete;
    }

    //キャンセル可能
    if (timer_ > 0.2f)
    {
        return ComboSystemInterface::State::Cancellation;
    }

    return ComboSystemInterface::State::NotCancellation;
}

void VitesseComboN1::Finalize()
{

}

void VitesseComboN2::Initialize()
{
    const auto owner = owner_.lock();
    if (!owner)return;

    //攻撃判定設定
    owner->BeginAttack("Slash_N_1");

    //アニメーション再生
    owner->PlayAnimation(Vitesse::AnimationIndex::Slash_N_2);
}

ComboSystemInterface::State VitesseComboN2::Run()
{
    const auto owner = owner_.lock();
    if (!owner)return ComboSystemInterface::State::NotCancellation;

    //アニメーション終了時にコンボ完了
    if (owner->GetAnimator()->GetAnimationFinished())
    {
        return ComboSystemInterface::State::Complete;
    }

    //キャンセル可能
    if (timer_ > 0.3f)
    {
        return ComboSystemInterface::State::Cancellation;
    }

    return ComboSystemInterface::State::NotCancellation;
}

void VitesseComboN2::Finalize()
{
}

void VitesseComboN3::Initialize()
{
    const auto owner = owner_.lock();
    if (!owner)return;

    //攻撃判定設定
    owner->BeginAttack("Slash_N_1");

    //アニメーション再生
    owner->PlayAnimation(Vitesse::AnimationIndex::Slash_N_3);
}

ComboSystemInterface::State VitesseComboN3::Run()
{
    const auto owner = owner_.lock();
    if (!owner)return ComboSystemInterface::State::NotCancellation;

    //アニメーション終了時にコンボ完了
    if (owner->GetAnimator()->GetAnimationFinished())
    {
        return ComboSystemInterface::State::Complete;
    }

    //キャンセル可能
    if (timer_ > 0.2f)
    {
        return ComboSystemInterface::State::Cancellation;
    }

    return ComboSystemInterface::State::NotCancellation;
}

void VitesseComboN3::Finalize()
{
}
