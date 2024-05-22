#include "Mouse.h"

using namespace AbyssEngine;

Mouse* Mouse::instance_;

Mouse::Mouse()
{
    mouse_ = std::make_unique<DirectX::Mouse>();
    if (instance_ == nullptr)
    {
        instance_ = this;
    }
    else
    {
        _ASSERT_EXPR(false, "AbyssEngine::Mouse�͂��łɐ�������Ă��܂��B");
    }
}

void Mouse::Update()
{
    //�X�V�O�Ƀ}�E�X�z�C�[���̉�]�l��ۑ�
    curMouseWheelValue_ = state_.scrollWheelValue;

    instance_->state_ = instance_->mouse_->GetState();
    tracker_.Update(state_);

    //�}�E�X�z�C�[����]�l�̌v�Z
    mouseWheelValue_ = state_.scrollWheelValue - curMouseWheelValue_;
}

DirectX::Mouse::State& Mouse::GetButtonState() 
{ 
    return instance_->state_; 
}

bool Mouse::GetButtonDown(const MouseButton button)
{
    DirectX::Mouse::ButtonStateTracker::ButtonState thisButton;
    auto tracker =  instance_->tracker_;
    switch (button)
    {
    case BTN_LEFT:  thisButton = tracker.leftButton;   break;
    case BTN_MIDDLE:thisButton = tracker.middleButton; break;
    case BTN_RIGHT: thisButton = tracker.rightButton;  break;
    default: return false; break;
    }
    return thisButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
}

bool Mouse::GetButtonUp(const MouseButton button)
{
    DirectX::Mouse::ButtonStateTracker::ButtonState thisButton;
    auto tracker = instance_->tracker_;
    switch (button)
    {
    case BTN_LEFT:  thisButton = tracker.leftButton;   break;
    case BTN_MIDDLE:thisButton = tracker.middleButton; break;
    case BTN_RIGHT: thisButton = tracker.rightButton;  break;
    default: return false; break;
    }
    return thisButton == DirectX::Mouse::ButtonStateTracker::RELEASED;
}

