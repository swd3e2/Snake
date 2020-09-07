#pragma once

#include "Graphics/Renderer.h"
#include <d3d11.h>
#include "DX11Window.h"
#include "DX11RenderContext.h"
#include "DX11MainRenderTarget.h"
#include "DX11Utils.h"

/**
 * DirectX renderer implementation
 */
class DX11Renderer : public Renderer 
{
private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	IDXGISwapChain* swapChain;

	std::unique_ptr<DX11Window> window;
	D3D11_VIEWPORT vp;
	ID3D11RasterizerState* m_RasterizerState;
	ID3D11RenderTargetView* nullRenderTargets[4] = { NULL };
	ID3D11ShaderResourceView* const nullShaderResourceView[1] = { NULL };
public:
	DX11Renderer();

	virtual ~DX11Renderer();

	virtual Window* createWindow(int width, int height) override;
	
	virtual void swapBuffers() override;

	virtual void drawIndexed(int cnt) override;

	virtual void draw(int cnt) override;

	virtual void setPrimitiveTopology(PrimitiveTopology topology) override;

	virtual void setViewport(int x0, int y0, int x1, int y1) override;

	virtual void setViewport(const Viewport& viewport) override;

	virtual void setDefaultDepthStencil() override;

	virtual void setDefaultBlendState() override;

	virtual void unbindResource(int slot) override;
};