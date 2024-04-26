#include "Keyboard.h"
#include "Engine.h"

using namespace AbyssEngine;

Keyboard* Keyboard::instance_;

Keyboard::Keyboard()
{
    keyboard_ = std::make_unique<DirectX::Keyboard>();
    if (instance_ == nullptr)
    {
        instance_ = this;
    }
    else
    {
        _ASSERT_EXPR(false, "AbyssEngine::Keyboard‚Í‚·‚Å‚É¶¬‚³‚ê‚Ä‚¢‚Ü‚·B");
    }
}

void Keyboard::Update()
{
    state_ = keyboard_->GetState();
    tracker_.Update(state_);
}

DirectX::Keyboard::State& AbyssEngine::Keyboard::GetKeyState()
{
    return instance_->state_;
}

const bool Keyboard::GetKeyDown(const DirectX::Keyboard::Keys key)
{
    return instance_->tracker_.IsKeyPressed(key);
}

const bool Keyboard::GetKeyUp(const DirectX::Keyboard::Keys key)
{
    return instance_->tracker_.IsKeyReleased(key);
}


