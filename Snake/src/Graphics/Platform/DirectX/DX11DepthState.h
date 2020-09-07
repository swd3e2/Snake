#pragma once

#include "Graphics/Renderer/DepthState.h"
#include "DX11Utils.h"
#include "DX11Renderer.h"

class DX11DepthState : public DepthState
{
private:
	ID3D11DepthStencilState* depthStencilState = nullptr;
public:
	DX11DepthState(const DepthStateDesc& desc) : DepthState(desc)
	{
		DX11Renderer* renderer = (DX11Renderer*)Renderer::instance();
		ID3D11Device* device = ((DX11RenderContext*)renderer->getContext())->getDevice();

		D3D11_DEPTH_STENCIL_DESC dssDesc = { 0 };
		dssDesc.DepthEnable = desc.depthEnable;
		dssDesc.DepthWriteMask = DirectX::getDepthWriteMask(desc.depthWriteMask);
		dssDesc.DepthFunc = DirectX::getComparisonFunction(desc.depthFunc);

		if (desc.stencilEnable) {
			dssDesc.StencilEnable = desc.stencilEnable;
			dssDesc.StencilReadMask = desc.stencilReadMask;
			dssDesc.StencilWriteMask = desc.stencilWriteMask;

			dssDesc.FrontFace.StencilDepthFailOp = DirectX::getDepthStencilOperation(desc.front.depthFailOp);
			dssDesc.FrontFace.StencilFailOp = DirectX::getDepthStencilOperation(desc.front.failOp);
			dssDesc.FrontFace.StencilPassOp = DirectX::getDepthStencilOperation(desc.front.passOp);
			dssDesc.FrontFace.StencilFunc = DirectX::getComparisonFunction(desc.front.stencilFunc);

			dssDesc.BackFace.StencilDepthFailOp = DirectX::getDepthStencilOperation(desc.back.depthFailOp);
			dssDesc.BackFace.StencilFailOp = DirectX::getDepthStencilOperation(desc.back.failOp);
			dssDesc.BackFace.StencilPassOp = DirectX::getDepthStencilOperation(desc.back.passOp);
			dssDesc.BackFace.StencilFunc = DirectX::getComparisonFunction(desc.back.stencilFunc);
		}

		if (device->CreateDepthStencilState(&dssDesc, &depthStencilState) != S_OK) {
			
		}
	}

	void bind(RenderContext* context)
	{
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)context)->getDeviceContext();
		deviceContext->OMSetDepthStencilState(depthStencilState, 0xFF);
	}
};