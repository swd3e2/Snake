#pragma once

#include "Graphics/Renderer/ShaderPipeline.h"
#include "DirectXRenderer.h"
#include <d3dcompiler.h>
#include "DirectXUtils.h"
#include "Graphics/Renderer/CommonTypes.h"

class DirectXShaderPipeline : public ShaderPipeline {
private:
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
public:
	DirectXShaderPipeline(const std::string& vertexShaderCode, const std::string& pixelShaderCode) :
		ShaderPipeline(vertexShaderCode, pixelShaderCode)
	{
		DirectXRenderer* renderer = (DirectXRenderer*)Renderer::instance();
		ID3D11Device* device = ((DirectXRenderContext*)renderer->getContext())->getDevice();
		ID3DBlob* shaderBlob = nullptr;

		if (!compileShaderInternal(vertexShaderCode, ShaderType::VERTEX, &shaderBlob)) {
			device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &vertexShader);
		} else {
			std::cout << "Failed to compile vertex shader" << std::endl;
		}
		shaderBlob->Release();

		if (!compileShaderInternal(pixelShaderCode, ShaderType::PIXEL, &shaderBlob)) {
			device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &pixelShader);
		} else {
			std::cout << "Failed to compile pixel shader" << std::endl;
		}
		shaderBlob->Release();
	}

	bool compileShaderInternal(const std::string& code, ShaderType type, ID3DBlob** blob) {
		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;

		const D3D_SHADER_MACRO defines[] = {
			{ "HLSL", "1" },
			{ nullptr, nullptr }
		};

		HRESULT hr = D3DCompile(code.c_str(), code.size(), NULL, defines, NULL,
			"main", DirectX::getShaderTarget(type).c_str(), D3DCOMPILE_ENABLE_STRICTNESS, // | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0, &shaderBlob, &errorBlob);

		if (FAILED(hr)) {
			if (errorBlob) {
				std::cout << errorBlob->GetBufferPointer() << std::endl;
				errorBlob->Release();
			}
			if (shaderBlob) {
				shaderBlob->Release();
			}

			return false;
		}

		*blob = shaderBlob;
		return true;
	}

	virtual void bind(RenderContext* renderContext) override {
		ID3D11DeviceContext* context = ((DirectXRenderContext*)renderContext)->getDeviceContext();
		context->VSSetShader(vertexShader, NULL, 0);
		context->PSSetShader(pixelShader, NULL, 0);
	}
};