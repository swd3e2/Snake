#pragma once

#include "Graphics/Renderer/ShaderInputLayout.h"
#include "DX11Renderer.h"
#include "DX11Utils.h"
#include <d3dcompiler.h>

class DX11ShaderInputLayout : public ShaderInputLayout {
private:
	ID3D11InputLayout* m_InputLayout = nullptr;
public:
	DX11ShaderInputLayout(const ShaderInputLayoutDesc& desc) {
		DX11Renderer* renderer = (DX11Renderer*)Renderer::instance();
		ID3D11Device* device = ((DX11RenderContext*)renderer->getContext())->getDevice();
		ID3D11DeviceContext* deviceContext = ((DX11RenderContext*)renderer->getContext())->getDeviceContext();

		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;

		std::string temp = "struct VertexShaderInput {";
		for (int i = 0; i < desc.elements.size(); i++) {
			temp += DirectX::getInputLayoutShaderFormat(desc.elements[i].type) + " attr" + std::to_string(i) + ": " + desc.elements[i].name + ";";
		}

		temp += "}; float4 main(VertexShaderInput input) : SV_POSITION { return float4(1.0f, 1.0f, 1.0f, 1.0f); }";


		const D3D_SHADER_MACRO defines[] =
		{
			{ "HLSL", "1" },
			{ nullptr, nullptr }
		};
		HRESULT hr = D3DCompile(temp.data(), temp.size(), NULL, defines, NULL, "main",
			"vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS,
			0, &shaderBlob, &errorBlob);

		ID3D11VertexShader* vertexTempShader;
		device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &vertexTempShader);

		D3D11_INPUT_ELEMENT_DESC* layout = new D3D11_INPUT_ELEMENT_DESC[desc.elements.size()];

		for (int i = 0; i < desc.elements.size(); i++) {
			if (i == 0) {
				layout[i] = { desc.elements[i].name.c_str(), 0, DirectX::getInputLayoutFormat(desc.elements[i].type), 0,	0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			} else {
				layout[i] = { desc.elements[i].name.c_str(), 0, DirectX::getInputLayoutFormat(desc.elements[i].type), 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}
		}

		device->CreateInputLayout(layout, desc.elements.size(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &m_InputLayout);

		if (errorBlob) errorBlob->Release();
		vertexTempShader->Release();
		shaderBlob->Release();

		delete[] layout;
	}

	virtual void bind(RenderContext* renderContext) override {
		ID3D11DeviceContext* context = ((DX11RenderContext*)renderContext)->getDeviceContext();
		context->IASetInputLayout(m_InputLayout);
	}
};