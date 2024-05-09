#include "Skybox.h"
#include "Shader.h"
#include "FullscreenQuad.h"
#include "DXSystem.h"

using namespace AbyssEngine;

Skybox::Skybox()
{
    pixelShader_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/SkyboxPS.cso");
}

void Skybox::Render(FullscreenQuad* bitBlockTransfer)
{
    DXSystem::SetBlendState(BS_State::Alpha);
    DXSystem::SetDepthStencilState(DS_State::None);
    DXSystem::SetRasterizerState(RS_State::Cull_None);
    bitBlockTransfer->Blit(nullptr, 0, 0, pixelShader_.Get());
}
