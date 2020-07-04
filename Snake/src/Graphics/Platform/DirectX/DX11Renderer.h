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
class DX11Renderer : public Renderer {
private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	IDXGISwapChain* swapChain;

	DX11Window* window;
	D3D11_VIEWPORT vp;
	ID3D11RasterizerState* m_RasterizerState;
	ID3D11RenderTargetView* nullRenderTargets[4] = { NULL };
	ID3D11ShaderResourceView* const nullShaderResourceView[1] = { NULL };
public:
	DX11Renderer() {
		_rendererType = RendererType::DirectX;
	}

	virtual Window* createWindow(int width, int height) override {
		vp.Width = (float)width;
		vp.Height = (float)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;

		window = new DX11Window(width, height);
		DX11MainRenderTarget* mainRenderTarget = new DX11MainRenderTarget(width, height);
		_mainRenderTarget = mainRenderTarget;

		DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
		swapChainDesc.BufferCount = 2;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferDesc.Height = height;
		swapChainDesc.BufferDesc.Width = width;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.OutputWindow = (window->getHwnd());

		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};

		D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_11_1;
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;

		HRESULT hr = S_OK;

		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags,
			featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &swapChainDesc, &swapChain,
			&device, &m_featureLevel, &deviceContext);

		_renderContext = new DX11RenderContext(device, deviceContext, swapChain);

		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&mainRenderTarget->m_BackBuffer);
		hr = device->CreateRenderTargetView(mainRenderTarget->m_BackBuffer, NULL, &mainRenderTarget->m_RenderTarget);

		D3D11_TEXTURE2D_DESC desc;
		desc.ArraySize = 1;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		desc.Width = width;

		hr = device->CreateTexture2D(&desc, NULL, &mainRenderTarget->m_DepthStencilBuffer);

		D3D11_DEPTH_STENCIL_VIEW_DESC ddesc = {};
		ddesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		ddesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		ddesc.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(mainRenderTarget->m_DepthStencilBuffer, &ddesc, &mainRenderTarget->m_DepthStencilView);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
		srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		srvd.Texture2D.MostDetailedMip = 0;

		hr = device->CreateShaderResourceView(mainRenderTarget->m_DepthStencilBuffer, &srvd, &mainRenderTarget->m_DepthResourceView);

		deviceContext->RSSetViewports(1, &vp);
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D11_RASTERIZER_DESC rasterizerDesc { };
		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		rasterizerDesc.FrontCounterClockwise = true;
		device->CreateRasterizerState(&rasterizerDesc, &m_RasterizerState);

		deviceContext->RSSetState(m_RasterizerState);

		return window;
	}
	
	virtual void swapBuffers() override {
		swapChain->Present(false, 0);
	}

	virtual void drawIndexed(int cnt) override {
		deviceContext->DrawIndexed(cnt, 0, 0);
	}

	virtual void draw(int cnt) override {
		deviceContext->Draw(cnt, 0);
	}

	virtual void setPrimitiveTopology(PrimitiveTopology topology) override {
		deviceContext->IASetPrimitiveTopology(DirectX::getTopology(topology));
	}

	virtual void setViewport(int x0, int y0, int x1, int y1) override {
		vp.Width = (float)x1;
		vp.Height = (float)y1;
		vp.TopLeftX = x0;
		vp.TopLeftY = y0;

		deviceContext->RSSetViewports(1, &vp);
	}

	virtual void setViewport(const Viewport& viewport) override {
		vp.Width = (float)viewport.x1;
		vp.Height = (float)viewport.y1;
		vp.TopLeftX = viewport.x0;
		vp.TopLeftY = viewport.y0;

		deviceContext->RSSetViewports(1, &vp);
	}

	virtual void unbindResource(int slot) override {
		deviceContext->PSSetShaderResources(slot, 1, nullShaderResourceView);
	}
};