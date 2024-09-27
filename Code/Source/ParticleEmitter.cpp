#include "ParticleEmitter.h"
#include "Actor.h"
#include "Engine.h"
#include "RenderManager.h"
#include "Texture.h"
#include "DxSystem.h"

#include <imgui/imgui.h>

using namespace AbyssEngine;

void ParticleEmitter::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();

    Engine::renderManager_->Add(std::static_pointer_cast<ParticleEmitter>(shared_from_this()));

    particleSystem_ = std::make_unique<ParticleSystem>(100);
    particleSystem_->Initialize();
    Texture::LoadTextureFromFile("./Assets/Effects/Texture/Particle01.png", particleTexture_.GetAddressOf(), NULL);
    particleSystem_->particleSystemData_.spriteSheetGrid_ = { 1, 1 };

    Texture::LoadTextureFromFile("./Assets/Effects/_noise_3d.dds", noise3d_.GetAddressOf(), NULL);


}

void AbyssEngine::ParticleEmitter::Render()
{
    DXSystem::SetDepthStencilState(DS_State::LEqual_No_Write);
    DXSystem::SetRasterizerState(RS_State::Cull_None);
    DXSystem::SetBlendState(BS_State::Add);

    DXSystem::GetDeviceContext()->PSSetShaderResources(0, 1, particleTexture_.GetAddressOf());
    //immediate_context->GSSetShaderResources(0, 1, color_temper_chart.GetAddressOf());
    DXSystem::GetDeviceContext()->GSSetShaderResources(1, 1, noise3d_.GetAddressOf());

    particleSystem_->Render();

}

void AbyssEngine::ParticleEmitter::RecalculateFrame()
{
    DXSystem::GetDeviceContext()->CSSetShaderResources(1, 1, noise3d_.GetAddressOf());
    particleSystem_->Integrate();
}

void AbyssEngine::ParticleEmitter::DrawImGui()
{
    if (ImGui::TreeNode("EffectEmitter"))
    {
        //particleSystem_->

        ImGui::TreePop();
    }
}
