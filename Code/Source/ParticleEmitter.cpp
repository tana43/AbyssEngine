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

    particleSystem_ = std::make_unique<ParticleSystem>(1000);
    particleSystem_->Initialize(actor_->GetDeltaTime());
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
    particleSystem_->Integrate(actor_->GetDeltaTime());
}

void AbyssEngine::ParticleEmitter::DrawImGui()
{
    if (ImGui::TreeNode("EffectEmitter"))
    {
        auto& data = particleSystem_->particleSystemData_;
        ImGui::ColorEdit4("Emission_Color", &data.emissionColor_.x, ImGuiColorEditFlags_PickerHueWheel);

        ImGui::DragFloat("Gravity", &data.gravity_, 0.01f);

        ImGui::DragFloat("Lifespan min", &data.flashLifespan_.x, 0.01f);
        ImGui::DragFloat("Lifespan min", &data.flashLifespan_.y, 0.01f);

        ImGui::DragFloat("Spawn delay min", &data.spawnDelay_.x, 0.01f);
        ImGui::DragFloat("Spawn delay max", &data.spawnDelay_.y, 0.01f);
        ImGui::DragFloat("Fade in duration", &data.fadeDuration_.x, 0.01f );
        ImGui::DragFloat("Fade out duration", &data.fadeDuration_.y, 0.01f );
        ImGui::DragFloat("Emission size spawn", &data.emissionSize_.x, 0.01f );
        ImGui::DragFloat("Emission size despawn", &data.emissionSize_.y, 0.01f);
        ImGui::DragFloat("Emission speed min", &data.emissionSpeed_.x, 0.01f );
        ImGui::DragFloat("Emission speed max", &data.emissionSpeed_.y, 0.01f );
        ImGui::DragFloat("Emission angular_speed min", &data.emissionAngularSpeed_.x, 0.01f);
        ImGui::DragFloat("Emission angular_speed max", &data.emissionAngularSpeed_.y, 0.01f);
        ImGui::DragFloat("Emission cone angle min", &data.emissionConeAngle_.x, 0.01f, 0.0f, +3.141592653f );
        ImGui::DragFloat("Emission cone angle max", &data.emissionConeAngle_.y, 0.01f, 0.0f, +3.141592653f );
        ImGui::SliderFloat("noiseScale", &data.noiseScale_, +0.0f, +1.0f);

        ImGui::TreePop();
    }
}
