#pragma once

#include "DX11Renderer.h"
#include "Graphics/Renderer/VertexBuffer.h"

class DX11VertexBuffer : public VertexBuffer {
private:
	ID3D11Buffer* m_Buffer;
public:
	DX11VertexBuffer(int size, int stride, void* data, bool isDynamic) :
		VertexBuffer(size, stride, isDynamic)
	{
		D3D11_BUFFER_DESC bufferDesc = { };
		bufferDesc.ByteWidth = stride * size;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		if (isDynamic) {
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
		}


		DX11Renderer* renderer = (DX11Renderer*)Renderer::instance();
		ID3D11Device* device = ((DX11RenderContext*)renderer->getContext())->getDevice();
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)renderer->getContext())->getDeviceContext();

		D3D11_SUBRESOURCE_DATA bufferData;
		bufferData.pSysMem = data;
		bufferData.SysMemPitch = 0;
		bufferData.SysMemSlicePitch = 0;

		device->CreateBuffer(&bufferDesc, &bufferData, &m_Buffer);
	}

	virtual ~DX11VertexBuffer() {
		if (m_Buffer) {
			m_Buffer->Release();
		}
	}

	virtual void update(void* data, size_t size, size_t stride) override {
		this->stride = stride;
		this->size = size;

		DX11RenderContext* r = static_cast<DX11RenderContext*>(Renderer::instance()->getContext());
		ID3D11Device* device = r->getDevice();
		ID3D11DeviceContext* deviceContext = r->getDeviceContext();

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		deviceContext->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, data, size * stride);
		deviceContext->Unmap(m_Buffer, 0);
	}

	virtual void bind(RenderContext* renderContext) {
		ID3D11DeviceContext* context = ((DX11RenderContext*)renderContext)->getDeviceContext();
		const unsigned int stride = getStride();
		const unsigned int offset = 0;
		context->IASetVertexBuffers(0, 1, &m_Buffer, &stride, &offset);
	}
};