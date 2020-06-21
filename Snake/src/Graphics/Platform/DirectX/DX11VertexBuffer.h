#pragma once

#include "DX11Renderer.h"
#include "Graphics/Renderer/VertexBuffer.h"

class DX11VertexBuffer : public VertexBuffer {
private:
	ID3D11Buffer* m_Buffer;
public:
	DX11VertexBuffer(int size, int stride, void* data) :
		VertexBuffer(size, stride)
	{
		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.ByteWidth = stride * size;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		/*if (desc.isDynamic) {
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
		}*/

		D3D11_SUBRESOURCE_DATA bufferData;
		bufferData.pSysMem = data;
		bufferData.SysMemPitch = 0;
		bufferData.SysMemSlicePitch = 0;

		DX11Renderer* renderer = (DX11Renderer*)Renderer::instance();
		ID3D11Device* device = ((DX11RenderContext*)renderer->getContext())->getDevice();
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)renderer->getContext())->getDeviceContext();

		device->CreateBuffer(&bufferDesc, &bufferData, &m_Buffer);
	}

	virtual void bind(RenderContext* renderContext) {
		ID3D11DeviceContext* context = ((DX11RenderContext*)renderContext)->getDeviceContext();
		const unsigned int stride = getStride();
		const unsigned int offset = 0;
		context->IASetVertexBuffers(0, 1, &m_Buffer, &stride, &offset);
	}
};