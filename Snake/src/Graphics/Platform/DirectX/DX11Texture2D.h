#pragma once

#include "Graphics/Renderer/Texture2D.h"
#include "DX11Renderer.h"
#include <iostream>
#include "DX11Utils.h"

class DX11Texture2D : public Texture2D {
public:
	ID3D11ShaderResourceView* m_TextureShaderResource = nullptr;
	ID3D11Texture2D* m_Texture = nullptr;
public:
	DX11Texture2D(int width, int height, int location, void* data, TextureFormat textureFormat, int numMips) :
		Texture2D(width, height, location, textureFormat, numMips)
	{
		UINT32 numFaces = 1;

		DX11Renderer* renderer = (DX11Renderer*)Renderer::instance();
		ID3D11Device* device = ((DX11RenderContext*)renderer->getContext())->getDevice();
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)renderer->getContext())->getDeviceContext();

		D3D11_TEXTURE2D_DESC textureDesc = {0};
		textureDesc.Height = height;
		textureDesc.Width = width;
		textureDesc.MipLevels = numMips + 1;
		textureDesc.ArraySize = numFaces == 0 ? 1 : numFaces;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Format = DirectX::getTextureFormat(textureFormat);
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		if (textureFormat == TextureFormat::D32 || textureFormat == TextureFormat::D16 || textureFormat == TextureFormat::D24S8 || textureFormat == TextureFormat::D32_S8X24) {
			textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		}

		//textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		textureDesc.CPUAccessFlags = 0;

		if (device->CreateTexture2D(&textureDesc, nullptr, &m_Texture) != S_OK) {
			std::cout << "Cant create texture 2D" << std::endl;
			return;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = numMips + 1;
		srvDesc.Format = textureDesc.Format;

		if (textureFormat == TextureFormat::D32) {
			srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		} else if (textureFormat == TextureFormat::D24S8) {
			srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		}

		// Create the shader resource view for the texture.
		if (device->CreateShaderResourceView(m_Texture, &srvDesc, &m_TextureShaderResource) != S_OK) {
			std::cout << "Cant create shader resource view" << std::endl;
			return;
		}

		if (data != nullptr) {
			D3D11_BOX box;
			box.left = 0;
			box.right = width;
			box.bottom = height;
			box.top = 0;
			box.front = 0;
			box.back = 1;

			deviceContext->UpdateSubresource(m_Texture, 0, &box, data, DirectX::getPitch(textureFormat, width), 1);
		}
	}

	virtual void bind(RenderContext* renderContext) override {
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)renderContext)->getDeviceContext();
		deviceContext->PSSetShaderResources(getLocation(), 1, &m_TextureShaderResource);
	}

	virtual void setData(void* data) override {
		DX11Renderer* renderer = (DX11Renderer*)Renderer::instance();
		ID3D11Device* device = ((DX11RenderContext*)renderer->getContext())->getDevice();
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)renderer->getContext())->getDeviceContext();

		D3D11_BOX box;
		box.left = 0;
		box.right = width;
		box.bottom = height;
		box.top = 0;
		box.front = 0;
		box.back = 1;

		deviceContext->UpdateSubresource(m_Texture, 0, &box, data, DirectX::getPitch(textureFormat, width), 1);
	}

	virtual void generateMips() override {

	}

	virtual void bindToUnit(const int location, RenderContext* renderContext) override {
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)renderContext)->getDeviceContext();
		deviceContext->PSSetShaderResources(location, 1, &m_TextureShaderResource);
	}
};