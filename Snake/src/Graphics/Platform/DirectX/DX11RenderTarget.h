#pragma once

#include "Graphics/Renderer/RenderTarget.h"
#include "DX11Renderer.h"
#include <vector>
#include "DX11Utils.h"
#include "DX11Texture2D.h"

class DX11RenderTarget : public RenderTarget {
private:
	std::vector<ID3D11RenderTargetView*> renderTargets;
	ID3D11DepthStencilView* m_DepthStencilView = nullptr;
	ID3D11RenderTargetView* nullRenderTargets[4] = { NULL };
	std::vector<Texture2D*> renderTargetTextures;
public:
	virtual void clear(RenderContext* renderContext) override {
		DX11Renderer* renderer = (DX11Renderer*)Renderer::instance();
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)renderer->getContext())->getDeviceContext();
		
		float color[4] = { 0,0,0,0 };
		for (auto& it : renderTargets) {
			deviceContext->ClearRenderTargetView(it, color);
		}
		if (m_DepthStencilView != nullptr) {
			deviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}

	virtual void setColorTexture(const std::shared_ptr<Texture2D>& texture, int slot, int level = 0) override {
		DX11Renderer* renderer = (DX11Renderer*)Renderer::instance();
		ID3D11Device* device = ((DX11RenderContext*)renderer->getContext())->getDevice();
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)renderer->getContext())->getDeviceContext();

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = DirectX::getTextureFormat(texture->getFormat());
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		ID3D11RenderTargetView* renderTarget = nullptr;
		device->CreateRenderTargetView(std::static_pointer_cast<DX11Texture2D>(texture)->m_Texture, &renderTargetViewDesc, &renderTarget);
		renderTargets.push_back(renderTarget);
		renderTargetTextures.push_back(texture.get());
	}

	virtual void removeColorTexture(int slot, int level) override {

	}

	virtual void setDepthTexture(const std::shared_ptr<Texture2D>& texture, int level = 0) override {
		DX11Renderer* renderer = (DX11Renderer*)Renderer::instance();
		ID3D11Device* device = ((DX11RenderContext*)renderer->getContext())->getDevice();
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)renderer->getContext())->getDeviceContext();

		D3D11_DEPTH_STENCIL_VIEW_DESC ddesc = {};
		ddesc.Format = DirectX::getDepthResourceFormat(texture->getFormat());
		ddesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		ddesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		ddesc.Texture2D.MipSlice = 0;
		ddesc.Flags = 0;

		device->CreateDepthStencilView(std::static_pointer_cast<DX11Texture2D>(texture)->m_Texture, &ddesc, &m_DepthStencilView);
		renderTargetTextures.push_back(texture.get());
	}

	virtual void bind(RenderContext* renderContext) override {
		DX11RenderContext* context = (DX11RenderContext*)renderContext;
		ID3D11DeviceContext* deviceContext = context->getDeviceContext();

		for (int i = 0; i < context->boundTextures.size(); i++) {
			for (auto& it : renderTargetTextures) {
				if (context->boundTextures[i] != nullptr && context->boundTextures[i] == it) {
					Renderer::instance()->unbindResource(i);
				}
			}
		}

		deviceContext->OMSetRenderTargets(4, nullRenderTargets, nullptr);
		deviceContext->OMSetRenderTargets(renderTargets.size(), renderTargets.data(), m_DepthStencilView);
		context->boundRenderTarget = this;
	}
};