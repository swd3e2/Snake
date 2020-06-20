#pragma once

#include "Graphics/Renderer.h"
#include <d3d11.h>
#include "DirectXWindow.h"
#include "DirectXRenderContext.h"

class DirectXRenderer : public Renderer {
private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	IDXGISwapChain* swapChain;

	DirectXWindow* window;
	DirectXRenderContext* context;
	D3D11_VIEWPORT vp;

	ID3D11RenderTargetView* m_RenderTarget;
	ID3D11DepthStencilView* m_DepthStencilView;

	ID3D11Texture2D* m_BackBuffer;
	ID3D11Texture2D* m_DepthStencilBuffer;
	ID3D11ShaderResourceView* m_DepthResourceView;
public:
	DirectXRenderer() {
		
	}

	virtual Window* createWindow(int width, int height) override {
		window = new DirectXWindow(width, height);

		DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
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

		context = new DirectXRenderContext(device, deviceContext, swapChain);

		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_BackBuffer);
		hr = device->CreateRenderTargetView(m_BackBuffer, NULL, &m_RenderTarget);

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

		hr = device->CreateTexture2D(&desc, NULL, &m_DepthStencilBuffer);

		D3D11_DEPTH_STENCIL_VIEW_DESC ddesc = {};
		ddesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		ddesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		ddesc.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(m_DepthStencilBuffer, &ddesc, &m_DepthStencilView);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
		srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		srvd.Texture2D.MostDetailedMip = 0;

		hr = device->CreateShaderResourceView(m_DepthStencilBuffer, &srvd, &m_DepthResourceView);

		deviceContext->RSSetViewports(1, &vp);
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		return window;
	}
	
	virtual void swapBuffers() override {
		swapChain->Present(false, 0);
	}

	virtual void drawIndexed(int cnt) override {
		deviceContext->DrawIndexed(cnt, 0, 0);
	}
};