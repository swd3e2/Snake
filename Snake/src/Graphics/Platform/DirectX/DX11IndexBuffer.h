#pragma once

#include "Graphics/Renderer/IndexBuffer.h"
#include "DX11Renderer.h"

class DX11IndexBuffer : public IndexBuffer {
private:
	ID3D11Buffer* m_Buffer;
public:
	DX11IndexBuffer(int size, void* data) :
		IndexBuffer(size)
	{
		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.ByteWidth = 4 * size;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
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

	virtual void bind(RenderContext* renderContext) override {
		ID3D11DeviceContext* context = ((DX11RenderContext*)renderContext)->getDeviceContext();
		context->IASetIndexBuffer(m_Buffer, DXGI_FORMAT_R32_UINT, 0);
	}
};