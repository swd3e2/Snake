#pragma once

#include "Graphics/Renderer/ConstantBuffer.h"
#include "DX11Renderer.h"

class DX11ConstantBuffer : public ConstantBuffer {
private:
public:
	ID3D11Buffer* m_Buffer;
public:
	DX11ConstantBuffer(int pos, size_t size, void* data)
		: ConstantBuffer(pos, size) 
	{
		HRESULT result = S_OK;
		unsigned int usize = static_cast<UINT>(size + (16 - size % 16)); // Align with 16 bytes

		DX11Renderer* renderer = (DX11Renderer*)Renderer::instance();
		ID3D11Device* device = ((DX11RenderContext*)renderer->getContext())->getDevice();

		// Constant buffer
		D3D11_BUFFER_DESC constantBufferDesc;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.ByteWidth = usize;
		constantBufferDesc.MiscFlags = 0;
		constantBufferDesc.StructureByteStride = 0;

		device->CreateBuffer(&constantBufferDesc, NULL, &m_Buffer);
	}

	virtual void bind(RenderContext* context) override {
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)context)->getDeviceContext();
		deviceContext->VSSetConstantBuffers(this->getPosition(), 1, &m_Buffer);
		deviceContext->PSSetConstantBuffers(this->getPosition(), 1, &m_Buffer);
	}

	virtual void update(void* data) override {
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		DX11Renderer* renderer = (DX11Renderer*)Renderer::instance();
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)renderer->getContext())->getDeviceContext();

		HRESULT hr = deviceContext->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		CopyMemory(mappedResource.pData, data, getSize());
		deviceContext->Unmap(m_Buffer, 0);
	}
};