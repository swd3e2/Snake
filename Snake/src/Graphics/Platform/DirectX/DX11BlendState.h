#pragma once

#include "Graphics/Renderer.h"
#include "Graphics/Renderer/BlendState.h"
#include "DX11Renderer.h"
#include "DX11Utils.h"

class DX11BlendState : public BlendState
{
private:
	ID3D11BlendState* blendState = nullptr;
public:
	DX11BlendState(const BlendStateDesc& desc) : BlendState(desc)
	{
		DX11Renderer* renderer = (DX11Renderer*)Renderer::instance();
		ID3D11Device* device = ((DX11RenderContext*)renderer->getContext())->getDevice();

		D3D11_BLEND_DESC blendDesc = { 0 };
		blendDesc.AlphaToCoverageEnable = desc.alphaToCoverageEnable;
		blendDesc.IndependentBlendEnable = desc.independentBlendEnable;

		for (int i = 0; i < desc.count; i++) {
			blendDesc.RenderTarget[i].BlendEnable = desc.blend[i].blendEnabled;
			blendDesc.RenderTarget[i].SrcBlend = DirectX::getBlend(desc.blend[i].srcBlend);
			blendDesc.RenderTarget[i].DestBlend = DirectX::getBlend(desc.blend[i].dstBlend);
			blendDesc.RenderTarget[i].BlendOp = DirectX::getBlendOperation(desc.blend[i].blendOp);
			blendDesc.RenderTarget[i].SrcBlendAlpha = DirectX::getBlend(desc.blend[i].srcBlendAlpha);
			blendDesc.RenderTarget[i].DestBlendAlpha = DirectX::getBlend(desc.blend[i].dstBlendAlpha);
			blendDesc.RenderTarget[i].BlendOpAlpha = DirectX::getBlendOperation(desc.blend[i].blendOpAlpha);
			blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}

		if (device->CreateBlendState(&blendDesc, &blendState) != S_OK) {

		}
	}

	virtual void bind(RenderContext* context)
	{
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)context)->getDeviceContext();
		float temp[4] = { 1.0f, 1.0f ,1.0f ,1.0f };
		deviceContext->OMSetBlendState(blendState, temp, 0xFFFFFFFF);
	}
};