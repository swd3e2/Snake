#pragma once

#include "Graphics/Renderer/Sampler.h"
#include "DX11Renderer.h"
#include <iostream>
#include "DX11Utils.h"

class DX11Sampler : public Sampler {
private:
	ID3D11SamplerState* m_SamplerState;
public:
	DX11Sampler(AddressingMode addressingMode, FilterMode minFilterModel, FilterMode magFilterMode, FilterMode mipFilterMode, int maxAnisotropy, ComparisonFunction comparisonFunction, float minLOD, float maxLOD, float mipLODBias) :
		Sampler(addressingMode, minFilterModel, magFilterMode, mipFilterMode, maxAnisotropy, comparisonFunction, minLOD, maxLOD, mipLODBias)
	{
		DX11Renderer* renderer = (DX11Renderer*)Renderer::instance();
		ID3D11Device* device = ((DX11RenderContext*)renderer->getContext())->getDevice();

		D3D11_SAMPLER_DESC samplerDesc = {};

		samplerDesc.AddressU = DirectX::getAddressingMode(addressingMode);
		samplerDesc.AddressV = DirectX::getAddressingMode(addressingMode);
		samplerDesc.AddressW = DirectX::getAddressingMode(addressingMode);
		samplerDesc.ComparisonFunc = DirectX::getComparisonFunction(comparisonFunction);
		samplerDesc.MinLOD = minLOD;
		samplerDesc.MaxLOD = maxLOD;
		samplerDesc.MipLODBias = mipLODBias;
		samplerDesc.Filter = DirectX::getFilteringMode(minFilterModel, magFilterMode, mipFilterMode, comparisonFunction != ComparisonFunction::NEVER);
		samplerDesc.MaxAnisotropy = maxAnisotropy;

		if (FAILED(device->CreateSamplerState(&samplerDesc, &m_SamplerState))) {
			std::cout << ("Failed to create sampler state") << std::endl;
		}
	}

	virtual void bind(RenderContext* renderContext) {
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)renderContext)->getDeviceContext();
		deviceContext->PSSetSamplers(samplerUnit, 1, &m_SamplerState);
	}
};