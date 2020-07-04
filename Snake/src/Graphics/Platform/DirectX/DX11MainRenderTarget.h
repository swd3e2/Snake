#pragma once

#include "Graphics/Renderer/MainRenderTarget.h"
#include "DX11RenderContext.h"

class DX11MainRenderTarget : public MainRenderTarget {
public:
	ID3D11Texture2D* m_BackBuffer;
	ID3D11RenderTargetView* m_RenderTarget;
	ID3D11ShaderResourceView* m_DepthResourceView;

	ID3D11Texture2D* m_DepthStencilBuffer;
	ID3D11DepthStencilView* m_DepthStencilView;
public:
	DX11MainRenderTarget(int width, int height) :
		MainRenderTarget(width, height)
	{}

	virtual void clear(RenderContext* renderContext) override {
		ID3D11DeviceContext* context = ((DX11RenderContext*)renderContext)->getDeviceContext();
		float color[4] = { 0,0,0,0 };
		context->ClearRenderTargetView(m_RenderTarget, color);
		context->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	virtual void bind(RenderContext* renderContext) override {
		ID3D11DeviceContext* context = ((DX11RenderContext*)renderContext)->getDeviceContext();
		context->OMSetRenderTargets(1, &m_RenderTarget, m_DepthStencilView);
	}
};