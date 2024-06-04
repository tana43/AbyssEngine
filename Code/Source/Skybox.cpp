#include "Skybox.h"
#include "Shader.h"
#include "FullscreenQuad.h"
#include "DXSystem.h"
#include "RenderManager.h"

using namespace AbyssEngine;

Skybox::Skybox()
{
    pixelShader_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/SkyboxPS.cso");
}

void Skybox::Render(FullscreenQuad* bitBlockTransfer)
{
#if ENABLE_DIFFERD_RENDERING//ディファードレンダリング
    DXSystem::SetBlendState(BS_State::GBuffer);
#else
    DXSystem::SetBlendState(BS_State::Alpha);
#endif

    DXSystem::SetDepthStencilState(DS_State::None);
    DXSystem::SetRasterizerState(RS_State::Cull_None);
    bitBlockTransfer->Blit(nullptr, 0, 0, pixelShader_.Get());
}
