// UNIT.32
#include "FullscreenQuad.h"
#include "Shader.h"
#include "Misc.h"
#include "Engine.h"

using namespace AbyssEngine;

FullscreenQuad::FullscreenQuad()
{
	embeddedVertexShader_ = Shader<ID3D11VertexShader>::Emplace("./Resources/Shader/FullscreenQuadVS.cso", nullptr, nullptr, 0);
	embeddedPixelShader_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/FullscreenQuadPS.cso");
}

void FullscreenQuad::Blit(ID3D11ShaderResourceView** shaderResourceView, uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader)
{
	const auto& deviceContext = DXSystem::GetDeviceContext();
#if 0

	deviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->IASetInputLayout(nullptr);

	deviceContext->VSSetShader(embeddedVertexShader_.Get(), 0, 0);
	replacedPixelShader ? deviceContext->PSSetShader(replacedPixelShader, 0, 0) : deviceContext->PSSetShader(embeddedPixelShader_.Get(), 0, 0);

	deviceContext->PSSetShaderResources(startSlot, numViews, shaderResourceView);

	deviceContext->Draw(4, 0);
#else
	ID3D11ShaderResourceView* cachedShaderResourceViews[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
	deviceContext->PSGetShaderResources(startSlot, numViews, cachedShaderResourceViews);

	deviceContext->PSSetShaderResources(startSlot, numViews, shaderResourceView);

	deviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->IASetInputLayout(nullptr);

	deviceContext->VSSetShader(embeddedVertexShader_.Get(), 0, 0);
	replacedPixelShader ? deviceContext->PSSetShader(replacedPixelShader, 0, 0) : deviceContext->PSSetShader(embeddedPixelShader_.Get(), 0, 0);

	deviceContext->Draw(4, 0);

	deviceContext->PSSetShaderResources(startSlot, numViews, cachedShaderResourceViews);
	for (ID3D11ShaderResourceView* cachedShaderResourceView : cachedShaderResourceViews)
	{
		if (cachedShaderResourceView) cachedShaderResourceView->Release();
	}
#endif // 0
}
