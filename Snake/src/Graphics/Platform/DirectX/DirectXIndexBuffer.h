#pragma once

#include "Graphics/Renderer/IndexBuffer.h"
#include "DirectXRenderer.h"

class DirectXIndexBuffer : public IndexBuffer {
private:
	ID3D11Buffer* m_Buffer;
public:
	DirectXIndexBuffer(int size, void* data) :
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

		DirectXRenderer* renderer = (DirectXRenderer*)Renderer::instance();
		ID3D11Device* device = ((DirectXRenderContext*)renderer->getContext())->getDevice();
		ID3D11DeviceContext* deviceContext = ((DirectXRenderContext*)renderer->getContext())->getDeviceContext();

		device->CreateBuffer(&bufferDesc, &bufferData, &m_Buffer);
	}

	virtual void bind(RenderContext* renderContext) override {
		ID3D11DeviceContext* context = ((DirectXRenderContext*)renderContext)->getDeviceContext();
		context->IASetIndexBuffer(m_Buffer, DXGI_FORMAT_R32_UINT, 0);
	}
};