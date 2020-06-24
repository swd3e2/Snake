#pragma once

#include "Graphics/Renderer/RenderContext.h"
#include <d3d11.h>
#include <string>
#include <vector>

class DX11RenderTarget;
class Texture2D;

class DX11RenderContext : public RenderContext {
private:
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DeviceContext;
	IDXGISwapChain* m_SwapChain;

	unsigned int renderTargetsBound;
public:
	std::vector<Texture2D*> boundTextures;
	DX11RenderTarget* boundRenderTarget = nullptr;
public:
	DX11RenderContext(ID3D11Device* device, ID3D11DeviceContext* deviceContext, IDXGISwapChain* swapChain) :
		m_Device(device), m_DeviceContext(deviceContext), m_SwapChain(swapChain)
	{
		boundTextures.resize(16);
	}

	~DX11RenderContext() {
		m_Device->Release();
		m_DeviceContext->Release();
		m_SwapChain->Release();
	}

	inline ID3D11Device* getDevice() const { return m_Device; }
	inline ID3D11DeviceContext* getDeviceContext() const { return m_DeviceContext; }
	inline IDXGISwapChain* getSwapChain() const { return m_SwapChain; }
};